<?php

/**
 * @file
 *  The interface for all site phase plugins.
 */

interface SiteStatePluginInterface {
  public function retrieveCurrentState();

  public function applyState($state);
}