<?php
/**
 * @file
 * Form hook implementations.
 */

/**
 * Implements hook_form_alter().
 */
function apple_news_form_alter(&$form, &$form_state, $form_id) {
  module_load_include('inc', 'apple_news');

  // Get all possible entity types and bundles available for export.
  $entity_type_exports = apple_news_get_entity_types();
  foreach ($entity_type_exports as $module => $info) {

    foreach ($info['entity_types'] as $type) {
      foreach ($info['bundles'] as $bundle) {
        if ($form_id == $bundle . '_' . $type . '_form' && $info['enabled']) {

          // Load export object.
          $export = $info['#export'];

          $entity_channels = [];

          // Entity object.
          $entity_id = isset($form['nid']['#value']) ? (int) $form['nid']['#value'] : NULL;

          if (!empty($entity_id)) {

            $entity = entity_load_single($type, $entity_id);
            $results = apple_news_load_articles($entity, $type);

            // Collect article id, channel name and postdate for the table below.
            foreach ($results as $row) {
              $channel = apple_news_channel($row->channel_id);
              $entity_channels[$row->article_id] = [
                'channel' => $channel->name,
                'sections' => $channel->name,
                'postdate' => format_date($row->postdate, 'short'),
                'article_id' => $row->article_id,
                'channel_id' => $row->channel_id,
              ];
            }
          }

          // Entity form JS helper (hide/show Apple News entity form options).
          $form['#attached']['js'][] = drupal_get_path('module', 'apple_news') . '/js/form.js';

          // Add Apple News vertical tab.
          $form['apple_news'] = [
            '#type' => 'fieldset',
            '#title' => t('Apple News'),
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#group' => 'additional_settings',
            '#weight' => -10,
            '#attributes' => [
              'class' => ['apple-news-options'],
            ]
          ];

          if (empty($entity_id)) {
            $preview_message = t('You have to save this content in order to be able to preview the Apple News generated document.');
          }
          else {
            $preview_message = t('<a href="!link">Download</a> the Apple News generated document.',
              [
                '!link' => url('<front>'),
              ]
            );
          }

          $form['apple_news']['preview'] = [
            '#markup' => '<div>' . $preview_message . '</div><br/>',
            '#prefix' => '<div><strong>' . t('Preview') . '</strong></div>',
          ];

          $form['apple_news']['container'] = [
            '#type' => 'container',
            '#prefix' => '<strong>' . t('Feeds') . '</strong>',
          ];

          $api_channels = apple_news_channels(TRUE);
          foreach ($api_channels as $channel_id => $channel) {
            $form['apple_news']['container']['channel-' . $channel_id] = [
              '#type' => 'checkbox',
              '#title' => $channel['name'],
              '#attributes' => [
                'data-channel-id' => $channel_id
              ],
              '#default_value' => !empty($entity->{'channel-' . $channel_id}) ? $entity->{'channel-' . $channel_id} : '',
            ];
            if (count($channel['sections']) > 1) {
              foreach ($channel['sections'] as $section_id => $section) {
                $form['apple_news']['container']['section-' . $channel_id . '-' . $section_id] = [
                  '#type' => 'checkbox',
                  '#title' => $section['name'] . ' ' . (!empty($section['default']) ? t('(default)') : ''),
                  '#attributes' => [
                    'data-section-of' => $channel_id,
                    'class' => ['sections']
                  ],
                  '#default_value' => !empty($entity->{'section-'  . $channel_id . '-' . $section_id}) 
                    ? $entity->{'section-'  . $channel_id . '-' . $section_id} : '',
                ];
              }
            }
          }

          if (!empty($entity_channels)) {
            // If this article was already published we should give an
            // option to removed it from Apple News.
            $rows = [];
            foreach ($entity_channels as $article_id => $info) {
              $rows[] = [
                ['data' => $info['channel']],
                ['data' => $info['sections']],
                ['data' => $info['postdate']],
                ['data' => l(
                  t('Delete'), 'admin/config/content/apple-news/'
                    . $type . '/'
                    . $entity_id
                    . '/' . $info['article_id'] . '/'
                    . $info['channel_id']
                    . '/delete'
                  )
                ],
              ];
            }
            $form['apple_news']['info'] = [
              '#type' => 'fieldset',
              '#title' => t('Apple News published channels')
            ];
            $form['apple_news']['info']['table'] = [
              '#markup' => theme('table',
              [
                'header' => [
                  t('Feed'),
                  t('Sections'),
                  t('Post date'),
                  ''
                ],
                'rows' => $rows
              ])
            ];
          }

        }
      }
    }

  }

}