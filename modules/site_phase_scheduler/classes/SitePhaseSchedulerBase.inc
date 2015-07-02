<?php

/**
 * @file
 *  Base class for all the scheduler classes.
 */

abstract class SitePhaseSchedulerBase implements SitePhaseSchedulerInterface {

  protected $spsid;

  protected $machine_name;

  protected $name;

  protected $is_recurring;

  protected $starts_on;

  protected $ends_on;

  /**
   * Constructor of the class.
   *
   * @param array $params
   *  An array of parameters representing the attributes of the class.
   */
  public function __construct($params) {
    foreach (array('spsid', 'machine_name', 'name', 'is_recurring', 'starts_on', 'ends_on') as $attribute) {
      $this->{$attribute} = !empty($params[$attribute]) ? $params[$attribute] : NULL;
    }
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::calculateSecondsFromStart()
   */
  public function calculateSecondsFromStart($time = NULL) {
    // Use the current time by default;
    if (empty($time)) {
      $time = REQUEST_TIME;
    }
    // If the time is not between the start and the end date, we just return
    // FALSE.
    $start_timestamp = strtotime($this->getStartsOn());
    if ($time < $start_timestamp) {
      return FALSE;
    }
    $ends_on = $this->getEndsOn();
    if (!empty($ends_on)) {
      $end_timestamp = strtotime($ends_on);
      if ($time > $end_timestamp) {
        return FALSE;
      }
    }

    // @todo: after the recurring will be implemented, we have to handle these
    // cases.
    return $time - $start_timestamp;
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::getMachineName()
   */
  public function getMachineName() {
    return $this->machine_name;
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::getSpsid()
   */

  public function getSpsid() {
    return $this->spsid;
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::getName()
   */
  public function getName() {
    return $this->name;
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::isRecurring()
   */
  public function isRecurring() {
    return $this->is_recurring;
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::getStartsOn()
   */
  public function getStartsOn() {
    return $this->starts_on;
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::getEndsOn()
   */
  public function getEndsOn() {
    return $this->ends_on;
  }

  /**
   * __toString() implementation.
   */
  public function __toString() {
    $output = '<strong>' . $this->getName() . "</strong>\n";
    $output .= t('starts on: !date', array('!date' => $this->getStartsOn())) . "\n";
    $ends_ond = $this->getEndsOn();
    if (!empty($ends_ond)) {
      $output .= t('ends on: !date', array('!date' => $ends_ond));
    }
    return nl2br($output);
  }

  /**
   * (non-PHPdoc)
   * @see SitePhaseSchedulerInterface::exportToSTD()
   */
  public function exportToSTD() {
    $std = new stdClass();
    foreach (array('spsid', 'machine_name', 'name', 'is_recurring', 'starts_on', 'ends_on') as $attribute) {
      $std->{$attribute} = isset($this->{$attribute}) ? $this->{$attribute} : NULL;
    }
    return $std;
  }
}
