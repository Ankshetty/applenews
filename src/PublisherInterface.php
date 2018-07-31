<?php

namespace Drupal\applenews;

/**
 * Applenews publisher manager.
 */
interface PublisherInterface {

  public function getChannel($channel_id);

  public function getArticle();

  public function GetSection();

  public function GetSections();

  public function postArticle();

  public function updateArticle();

}
