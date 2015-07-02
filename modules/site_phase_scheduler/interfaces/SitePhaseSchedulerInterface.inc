<?php

/**
 * @file
 *  Interface for the site phase scheduler classes.
 */

interface SitePhaseSchedulerInterface {

  /**
   * Getter for the shceduler id field.
   */
  public function getSpsid();

  /**
   * Getter for the machine name field.
   */
  public function getMachineName();

  /**
   * Getter for the scheduler name.
   */
  public function getName();

  /**
   * Checks if the scheduler is a recurring one.
   */
  public function isRecurring();

  /**
   * Getter for the scheduler 'starts_on' field.
   */
  public function getStartsOn();

  /**
   * Getter for the scheduler 'ends_on' field.
   */
  public function getEndsOn();

  /**
   * Computes the number of seconds since the starting date of the scheduler and
   * a specific timestamp.
   *
   * This method will return FALSE when the time is outside the start-end
   * interval, so make sure you use an identical operator instead of equal to
   * test for 0. (use '===' and '!==' instead of '==' and '!=').
   * For example: if (someObj->calculateSecondsFromStart() !== FALSE) {
   *   ...
   * }
   */
  public function calculateSecondsFromStart($time = NULL);

  /**
   * Exports the object to an stdClass.
   */
  public function exportToSTD();

}
