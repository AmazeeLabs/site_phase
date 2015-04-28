<?php

/**
 * @file
 *  Factory for constructing site configuration plugins.
 */

class SiteConfigurationPluginFactory {

  /**
   * The maps of plugins. Each element is indexed by the plugin key, and it is
   * an array of properties. The 'class' property specifies the class to
   * instantiate.
   */
  protected $map;

  /**
   * Private constructor this is a singletone.
   */
  private function __construct() {
    // Construct the map.
    $this->map = site_phase_get_all_configuration_plugins();
  }

  /**
   * Returns an instance of the factory.
   *
   * @return SiteConfigurationPluginFactory
   */
  public static function getInstance() {
    static $instance;
    if (!isset($instance)) {
      $instance = new SiteConfigurationPluginFactory();
    }
    return $instance;
  }

  /**
   * Instantiates a plugin from a key.
   *
   * @return SiteConfigurationPluginInterface
   */
  public function getPlugin($key) {
    if (empty($this->map[$key])) {
      throw new SiteConfigurationPluginNotFoundException();
    }
    if (!class_exists($this->map[$key]['class'])) {
      throw new SiteConfigurationPluginClassNotFoundException();
    }
    $plugin = new $this->map[$key]['class']($key, $this->map[$key]);
    if (!$plugin instanceof SiteConfigurationPluginInterface) {
      throw new SiteConfigurationPluginInvalidClassException();
    }
    return $plugin;
  }
}

// Some exceptions which can be thrown by the factory class.

class SiteConfigurationPluginNotFoundException extends Exception {

}

class SiteConfigurationPluginClassNotFoundException extends Exception {

}

class SiteConfigurationPluginInvalidClassException extends Exception {

}
