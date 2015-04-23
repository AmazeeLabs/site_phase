<?php

/**
 * @file
 *  The interface for all site phase plugins.
 */

interface SiteStatePluginInterface {

  public function getId();

  public function getTitle();

  public function applyState($state);

  public function configurationForm($state = NULL);

  public function getStateFromSubmission(&$form, &$form_state);

}
