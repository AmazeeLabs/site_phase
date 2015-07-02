<?php

/**
 * @file
 *  Factory class for creating scheduler objects.
 */

class SitePhaseSchedulerFactory {

  public static function getScheduler(array $params) {
    return new SitePhaseScheduler($params);
  }
}