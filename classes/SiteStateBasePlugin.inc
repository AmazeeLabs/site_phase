<?php

/**
 * @file
 *  Base site state plugin class.
 */

abstract class SiteStateBasePlugin implements SiteStatePluginInterface {

  protected $id;

  protected $title;

  public function __construct($id, $title) {
    $this->id = $id;
    $this->title = $title;
  }

  public function getId() {
    return $this->id;
  }

  public function getTitle() {
    return $this->title;
  }
}
