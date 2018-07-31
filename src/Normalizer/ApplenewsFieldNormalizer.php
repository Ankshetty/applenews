<?php

namespace Drupal\applenews\Normalizer;

use Drupal\Core\Field\FieldItemListInterface;

class ApplenewsFieldNormalizer extends ApplenewsNormalizerBase {
  /**
   * {@inheritdoc}
   */
  public function supportsNormalization($data, $format = NULL) {
    return $format === $this->format && $data instanceof FieldItemListInterface;
  }

  /**
   * {@inheritdoc}
   */
  public function normalize($field, $format = NULL, array $context = []) {
    $value = '';

    foreach ($field as $field_item) {
      $value .= $this->serializer->normalize($field_item, $format, $context);
    }

    return $value;
  }
}