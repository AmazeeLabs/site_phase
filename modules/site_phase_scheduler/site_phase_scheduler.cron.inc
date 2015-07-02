<?php

/**
 * @file
 *  Implements the cron hook for the module.
 */

/**
 * Implements the cron hook.
 */
function _site_phase_scheduler_run_cron() {
  // In order to find a scheduler entry that matches the current time, we do
  // the following:
  // 1. We load all the schedulers, for all the phases.
  // 2. For each of them we get the number of seconds since the scheduler should
  // have been activated.
  // 3. If there is one ore more results which returned something else than
  // FALSE, then we consider the one with the smallest number of seconds.
  // 4. If there is no result, and the current site phase is not the default
  // one, then we switch to the default phase.
  // 5. If there is a result, and it is different than the current phase, then
  // we switch to it.

  // Step 1.
}
