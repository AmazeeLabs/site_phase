<?php

/**
 * @file
 *  Administration menu callbacks for the module.
 */

/**
 * Menu callback for the schedule phase page.
 */
function site_phase_scheduler_schedule_phase($phase) {
  $schedulers = SitePhaseSchedulerUtils::loadSchedulers($phase);
  $header = array(t('Schedule'), t('Actions'));
  $rows = array();
  if (!empty($schedulers)) {
    foreach ($schedulers as $scheduler) {
      $row = array();
      $row['description'] = array('data' => $scheduler->__toString());
      $actions = array();
      $actions[] = l(t('edit'), 'admin/config/system/site-phases/' . $phase->spid . '/schedule/' . $scheduler->getSpsid() . '/edit');
      $actions[] = l(t('delete'), 'admin/config/system/site-phases/' . $phase->spid . '/schedule/' . $scheduler->getSpsid() . '/delete');
      $row['actions'] = array('data' => implode(' | ', $actions));
      $rows[] = $row;
    }
  }
  return array(
    'schedule_list' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    ),
    'add_scheduler' => drupal_get_form('site_phase_schedule_add_edit_form', $phase),
  );
}

/**
 * Form constructor for the add/edit schedule.
 */
function site_phase_schedule_add_edit_form($form, &$form_state, $phase, SitePhaseSchedulerInterface $scheduler = NULL) {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Please input a name for the scheduler.'),
    '#default_value' => !empty($scheduler) ? $scheduler->getName() : '',
  );
  $form['is_recurring'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is recurring'),
    '#description' => t('If a scheduler is recurring, you will be able to define date patterns for when should the site phase be actived.') . ' NOT YET IMPLEMENTED!',
    '#default_value' => !empty($scheduler) ? $scheduler->isRecurring() : '',
  );
  $form['starts_on'] = array(
    '#title' => t('Starts on'),
    '#type' => 'date_popup',
    '#required' => TRUE,
    '#default_value' => !empty($scheduler) ? $scheduler->getStartsOn() : '',
  );
  $form['ends_on'] = array(
    '#title' => t('Ends on'),
    '#type' => 'date_popup',
    '#default_value' => !empty($scheduler) ? $scheduler->getEndsOn() : '',
  );
  $form['phase'] = array(
    '#type' => 'value',
    '#value' => $phase,
  );
  if (!empty($scheduler)) {
    $form['spsid'] = array(
      '#type' => 'value',
      '#value' => $scheduler->getSpsid(),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => !empty($scheduler) ? t('Update') : t('Add'),
  );
  return $form;
}

/**
 *
 */
function site_phase_schedule_add_edit_form_submit($form, &$form_state) {
  $parameters = array(
    'spsid' => !empty($form_state['values']['spsid']) ? $form_state['values']['spsid'] : NULL,
    'machine_name' => $form_state['values']['phase']->machine_name,
    'name' => $form_state['values']['name'],
    'is_recurring' => $form_state['values']['is_recurring'],
    'starts_on' => $form_state['values']['starts_on'],
    'ends_on' => $form_state['values']['ends_on'],
  );
  $new = !empty($form_state['values']['spsid']) ? FALSE : TRUE;
  $scheduler = SitePhaseSchedulerFactory::getScheduler($parameters);
  // @todo: we may need to change this in the future if we want to immediately
  // access the id of the new scheduler. The issue is that the scheduler object
  // does not get updated during the save because we actually just save the
  // export of it as an stdClass(). As a quick solution we could add a setter
  // method for the spsid in the class and call it from
  // SitePhaseSchedulerUtils::saveScheduler(), but that is a bad OOP use.
  // Another solution would be to first save the scheduler and then instantiate
  // the object, so the SitePhaseSchedulerUtils::saveScheduler() would receive
  // directly an stdClass instead of a SitePhaseSchedulerInterface object.
  SitePhaseSchedulerUtils::saveScheduler($scheduler);
  if ($new) {
    drupal_set_message(t('The scheduler has been created.'));
  }
  else {
    drupal_set_message(t('The scheduler has been updated.'));
  }
  $form_state['redirect'] = 'admin/config/system/site-phases/' . $form_state['values']['phase']->spid . '/schedule';
}

/**
 * Confirmation form for deleting a scheduler.
 */
function site_phase_schedule_delete_confirm($form, &$form_state, $phase, SitePhaseSchedulerInterface $scheduler) {
  $form['spsid'] = array(
    '#type' => 'value',
    '#value' => $scheduler->getSpsid(),
  );
  $form['spid'] = array(
    '#type' => 'value',
    '#value' => $phase->spid,
  );
  return confirm_form($form, t('Are you sure you want to delete the scheduler?'), 'admin/config/system/site-phases/' . $phase->spid . '/schedule',
      t('Are you sure you want to delete the %name scheduler? The action cannot be undone!', array('%name' => $scheduler->getName())));
}

/**
 * Submit handler for the scheduler delete confirmation form.
 */
function site_phase_schedule_delete_confirm_submit($form, &$form_state) {
  SitePhaseSchedulerUtils::deleteScheduler($form_state['values']['spsid']);
  drupal_set_message(t('The scheduler has been deleted.'));
  $form_state['redirect'] = 'admin/config/system/site-phases/' . $form_state['values']['spid'] . '/schedule';
}
