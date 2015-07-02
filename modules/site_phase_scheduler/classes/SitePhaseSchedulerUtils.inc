<?php

/**
 * @file
 *  Utils class for the site scheduler module.
 */

class SitePhaseSchedulerUtils {

  /**
   * Loads the schedulers of a phase.
   *
   * @param object $phase
   *  Optional, a site phase object. It must have a 'machine_name' attribute.
   */
  public static function loadSchedulers($phase = NULL) {
    $query = db_select('site_phases_scheduler', 'sps')->fields('sps');
    if (!empty($phase)) {
      $query->condition('sps.machine_name', $phase->machine_name);
    }
    $result = $query->execute()->fetchAll();
    $schedulers = array();
    foreach ($result as $row) {
      $params = (array) $row;
      $schedulers[$params['spsid']] = SitePhaseSchedulerFactory::getScheduler($params);
    }
    return $schedulers;
  }

  /**
   * Deletes a scheduler by its id.
   */
  public static function deleteScheduler($spsid) {
    db_delete('site_phases_scheduler')->condition('spsid', $spsid)->execute();
  }

  /**
   * Saves a scheduler into the database.
   */
  public static function saveScheduler(SitePhaseSchedulerInterface $scheduler) {
    $object = $scheduler->exportToSTD();
    $primary_keys = array();
    if (!empty($object->spsid)) {
      $primary_keys = array('spsid');
    }
    drupal_write_record('site_phases_scheduler', $object, $primary_keys);
  }
}
