<?php

/**
 * @file
 *  Implements the cron hook for the module.
 */

/**
 * Implements the cron hook.
 */
function _site_phase_scheduler_run_cron() {
  // Step 1: load all the schedulers, for all the phases.
  $schedulers = SitePhaseSchedulerUtils::loadSchedulers();

  // Step 2: find the best scheduler, if any, by checking the number of seconds
  // since each of the schedulers should have started. The one with the smallest
  // number is the best.
  $best_scheduler = NULL;
  $min_seconds = NULL;
  foreach ($schedulers as $scheduler) {
    $seconds = $scheduler->calculateSecondsFromStart();
    if ($seconds !== FALSE) {
      if (is_null($min_seconds) || $min_seconds > $seconds) {
        $min_seconds = $seconds;
        $best_scheduler = $scheduler;
      }
    }
  }

  // Step 4: If there is no result, and the current site phase is not the
  // default one, then we switch back to the default phase. However, we do this
  // only if the current phase does have at least one scheduler, otherwise this
  // means it was manually activated, so we do not switch back to default.
  $phase_to_activate = NULL;
  if (empty($best_scheduler)) {
    $default_site_phase = site_phase_default_load();
    if ($default_site_phase->machine_name != site_phase_get_current_phase()) {
      $current_phase_schedulers = SitePhaseSchedulerUtils::loadSchedulers(site_phase_machine_name_load(site_phase_get_current_phase()));
      if (!empty($current_phase_schedulers)) {
        $phase_to_activate = $default_site_phase;
      }
    }
  }
  else {
    // Step 5: If there is a result, and it is different than the current phase,
    // then we switch to it.
    if ($best_scheduler->getMachineName() != site_phase_get_current_phase()) {
      $phase = site_phase_machine_name_load($best_scheduler->getMachineName());
      $phase_to_activate = $phase;
    }
  }

  // Step 6: finally, if we have to activate any phase, we do it here.
  if (!empty($phase_to_activate)) {
    site_phase_activate_phase($phase_to_activate);
  }
}
