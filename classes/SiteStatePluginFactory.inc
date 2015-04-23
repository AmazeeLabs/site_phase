<?php

/**
 * @file
 *  Factory for constructing site state plugins.
 */

class SiteStatePluginFactory {

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
    $this->map = site_phase_get_all_state_plugins();
  }

  /**
   * Returns an instance of the factory.
   *
   * @return SiteStatePluginFactory
   */
  public static function getInstance() {
    static $instance;
    if (!isset($instance)) {
      $instance = new SiteStatePluginFactory();
    }
    return $instance;
  }

  /**
   * Instantiates a plugin from a key.
   *
   * @return SiteStatePluginInterface
   */
  public function getPlugin($key) {
    if (empty($this->map[$key])) {
      throw new SiteStatePluginNotFoundException();
    }
    if (!class_exists($this->map[$key]['class'])) {
      throw new SiteStatePluginClassNotFoundException();
    }
    $plugin = new $this->map[$key]['class']($key, $this->map[$key]['name']);
    if (!$plugin instanceof SiteStatePluginInterface) {
      throw new SiteStatePluginInvalidClassException();
    }
    return $plugin;
  }
}

// Some exceptions which can be thrown by the factory class.

class SiteStatePluginNotFoundException extends Exception {

}

class SiteStatePluginClassNotFoundException extends Exception {

}

class SiteStatePluginInvalidClassException extends Exception {

}
