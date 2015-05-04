<?php

/**
 * @file
 *  The interface for all site phase plugins.
 */

interface SiteConfigurationPluginInterface {

  public function getId();

  public function getTitle();

  public function applyConfiguration($configuration);

  public function getCurrentSiteConfiguration();

  public function getConfigurationAreas();

  public function configurationForm(&$form, &$form_state, $configuration_area, $default_phase_configuration = NULL, $configuration = NULL);

  public function getConfigurationFromSubmission($configuration_area, &$form, &$form_state);

}
