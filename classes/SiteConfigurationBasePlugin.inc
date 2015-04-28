<?php

/**
 * @file
 *  Base site configuration plugin class.
 */

abstract class SiteConfigurationBasePlugin implements SiteConfigurationPluginInterface {

  protected $id;

  protected $plugin_definition;

  public function __construct($id, $plugin_definition) {
    $this->id = $id;
    $this->plugin_definition = $plugin_definition;
  }

  public function getId() {
    return $this->id;
  }

  public function getTitle() {
    return $this->plugin_definition['title'];
  }

  public function getConfigurationAreas() {
    return $this->plugin_definition['configuration_areas'];
  }
}
