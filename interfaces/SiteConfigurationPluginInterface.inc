<?php

/**
 * @file
 *  The interface for all site phase plugins.
 */

interface SiteConfigurationPluginInterface {

  public function getId();

  public function getTitle();

  /**
   * Applies a configuration to the site.
   */
  public function applyConfiguration($configuration);

  /**
   * Merges the conf2 into $conf1.
   */
  public function mergeConfiguration($conf1, $conf2);

  public function getCurrentSiteConfiguration();

  public function getConfigurationAreas();

  public function configurationForm(&$form, &$form_state, $phase, $configuration_area, $default_phase_configuration = NULL, $configuration = NULL);

  public function getConfigurationFromSubmission($phase, $configuration_area, &$form, &$form_state);

}
