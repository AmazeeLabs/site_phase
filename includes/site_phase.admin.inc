<?php

/**
 * @file
 *  Menu callbacks for the module.
 */

/**
 * Menu callback for the main administration page of the phases.
 */
function site_phase_admin() {
  $all_phases = site_phase_get_all_phases();
  return array(
    'phases_table' => drupal_get_form('site_phase_admin_form', $all_phases),
    'add_form' => drupal_get_form('site_phase_add_edit_form'),
  );
}

/**
 * Form constructor for adding phases.
 */
function site_phase_add_edit_form($form, &$form_state, $site_phase = NULL) {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => !empty($site_phase) ? $site_phase->name : '',
    '#required' => TRUE,
    '#maxlength' => 128,
    '#description' => t('The name of the phase.'),
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => !empty($site_phase) ? $site_phase->machine_name : '',
    '#maxlength' => 64,
    '#machine_name' => array(
      'exists' => 'site_phase_machine_name_load', // @todo: to implement it.
      'source' => array('name')
    ),
    '#disabled' => !empty($site_phase),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => !empty($site_phase) ? $site_phase->description : '',
    '#description' => t('A short description of what the phase is about.'),
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => !empty($site_phase) ? $site_phase->weight : 0,
    '#delta' => 10,
  );
  if (!empty($site_phase)) {
    $form['spid'] = array(
      '#type' => 'value',
      '#value' => $site_phase->spid,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => !empty($site_phase) ? t('Update') : t('Save'),
  );
  return $form;
}

function site_phase_add_edit_form_submit(&$form, &$form_state) {
  // @todo: load a site phase if provided.
  $site_phase = new stdClass();
  if (!empty($form_state['values']['spid'])) {
    $site_phase = site_phase_load($form_state['values']['spid']);
  }
  $fields = array('name', 'machine_name', 'description', 'weight');
  foreach ($fields as $field_name) {
    $site_phase->{$field_name} = $form_state['values'][$field_name];
  }
  site_phase_save($site_phase);
  drupal_set_message(t('The site phase has been saved.'));
}

/**
 * Form constructor for the site phases overview form.
 */
function site_phase_admin_form($form, &$form_state, $phases) {
  $header = array('name' => t('Name'),
                  'machine_name' => t('Machine name'),
                  'description' => t('Description'),
                  'default' => t('Default'),
                  'actions' => t('Actions'),
                  'weight' => t('Weight'));
  $weight_delta = round(count($phases) / 2) + 10;
  $current_phase_name = site_phase_get_current_phase();
  foreach ($phases as $phase) {
    $row = array();
    $name = $phase->name;
    if ($phase->machine_name == $current_phase_name) {
      $name .= ' ' . t('(Currently active)');
    }
    $row['name'] = array('data' => $name);
    $row['machine_name'] = array('data' => $phase->machine_name);
    $row['description'] = array('data' => $phase->description);
    $row['default'] = array('data' => $phase->is_default ? t('YES') : '');
    $actions = array();
    // Add the edit and delete actions if the phase is not locked and if the
    // phase is not the active one.s
    // @todo: if we will implemente different permissions, then we have to
    // check them here.
    $destination = drupal_get_destination();
    $actions[] = l(t('edit'), site_phase_get_edit_path($phase->spid), array('query' => $destination));
    if (empty($phase->is_locked) && $current_phase_name != $phase->machine_name) {
      $actions[] = l(t('delete'), site_phase_get_delete_path($phase->spid), array('query' => $destination));
    }
    // Disabled for now. Could be removed or refactored in the future.
    //$actions[] = l(t('configure'), site_phase_get_configure_path($phase->spid), array('query' => $destination));
    $actions[] = l(t('activate'), site_phase_get_activate_path($phase->spid), array('query' => $destination));
    $actions[] = l(t('save current configuration'), site_phase_get_save_configuration_path($phase->spid), array('query' => $destination, 'attributes' => array('title' => t('Saves the current configuration to the phase.'))));
    $row['actions'] = array('data' => implode(' | ', $actions));

    $rows[$phase->spid] = array('data' => $row, 'class' => array('draggable'));
    $form['weight_' . $phase->spid] = array(
      '#type' => 'weight',
      '#default_value' => $phase->weight,
      '#delta' => $weight_delta,
      '#attributes' => array('class' => array('component-phase-weight')),
      '#title' => t('Weight'),
      '#title_display' => 'invisible',
    );
    $spids[] = $phase->spid;
  }
  $form['spids'] = array(
    '#type' => 'value',
    '#value' => $spids,
  );
  $form['#header'] = $header;
  $form['#rows'] = $rows;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for the site phase overview form.
 */
function site_phase_admin_form_submit(&$form, &$form_state) {
  foreach ($form_state['values']['spids'] as $spid) {
    $phase = site_phase_load($spid);
    // Update the weight and @todo: handle the update of the 'default';
    $phase->weight = $form_state['values']['weight_' . $spid];
    site_phase_save($phase);
  }
}

/**
 * Theme for the site_phase_admin_form form.
 */
function theme_site_phase_admin_form(&$variables) {
  $form = $variables['form'];
  $table_id = 'current-phases-table';
  foreach ($form['#rows'] as $spid => &$value) {
    $form['#rows'][$spid]['data']['weight'] = drupal_render($form['weight_' . $spid]);
  }
  $output = theme('table', array('header' => $form['#header'], 'rows' => $form['#rows'], 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'component-phase-weight');
  return $output;
}

/**
 * Confirmation form for activating a site phase.
 */
function site_phase_activate_confirm($form, &$form_state, $phase) {
  $form['new_phase_name'] = array(
    '#type' => 'value',
    '#value' => $phase->machine_name,
  );
  $form['confirmation'] = array(
    '#type' => 'checkbox',
    '#required' => TRUE,
    '#title' => t('Yes, I want to activate the %phase_name phase and I understand the consequences of this action.', array('%phase_name' => $phase->name)),
  );
  return confirm_form($form, t('Are you sure you want to change the site phase?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to activate the %phase_name site phase? IMPORTANT: This will activate all the settings of the %phase_name phase and will replace the ones from the current phase. If you are in the middle of configuring another phase, the changes will be lost.', array('%phase_name' => $phase->name)));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_activate_confirm_submit($form, &$form_state) {
  $phase = site_phase_machine_name_load($form_state['values']['new_phase_name']);
  site_phase_activate_phase($phase);
  // Check if the site phase was updated.
  $current_phase = site_phase_get_current_phase();
  if ($current_phase == $phase->machine_name) {
    drupal_set_message(t('The phase of the site has been activated.'));
  }
  else {
    // @todo: maybe handle some errors here.
    drupal_set_message(t('There was an error in activating the site phase. The site phase has not been activated.'), 'error');
  }
  $form_state['redirect'] = 'admin/config/system/site-phases';
}

/**
 * Confirmation form for configuring a site phase.
 */
function site_phase_configure_confirm($form, &$form_state, $phase) {
  $form['new_phase_name'] = array(
    '#type' => 'value',
    '#value' => $phase->machine_name,
  );
  $form['confirmation'] = array(
    '#type' => 'checkbox',
    '#required' => TRUE,
    '#title' => t('Yes, I want to configure the %phase_name phase and I understand the consequences of this action.', array('%phase_name' => $phase->name)),
  );
  return confirm_form($form, t('Are you sure you want to configure the site phase?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to configure the %phase_name site phase? IMPORTANT: The site will automatically be set in the %phase_name phase, and all the settings of the %phase_name phase will be active. If you are in the middle of configuring another phase, the changes will be lost.', array('%phase_name' => $phase->name)));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_configure_confirm_submit($form, &$form_state) {
  // @todo: remember the current state and go back to it after commit.
  $phase = site_phase_machine_name_load($form_state['values']['new_phase_name']);
  site_phase_activate_phase($phase);
  // Check if the site phase was updated.
  $current_phase = site_phase_get_current_phase();
  if ($current_phase == $phase->machine_name) {
    drupal_set_message(t('The phase of the site has been changed to %phase_name. You are now in configuration mode.', array('%phase_name' => $phase->name)));
    site_phase_set_configuration_mode();
  }
  else {
    // @todo: maybe handle some errors here.
    drupal_set_message(t('There was an error in changing the site phase. The configuration mode for %phase_name has NOT been initialized.', array('%phase_name' => $phase->name)), 'error');
  }
  $form_state['redirect'] = 'admin/config/system/site-phases';
}

/**
 * Confirmation form for configuration commit on the current site phase.
 */
function site_phase_configure_commit_confirm($form, &$form_state) {
  // @todo: would be nice to have a list of changes here.
  $form['confirmation'] = array(
    '#type' => 'checkbox',
    '#required' => TRUE,
    '#title' => t('Yes, I want to commit the changes I made to the current phase.'),
  );
  return confirm_form($form, t('Are you sure you want to commit the changes to the current phase?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to commit the changes to the current phase?'));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_configure_commit_confirm_submit($form, &$form_state) {
  site_phase_commit_configuration();
  drupal_set_message(t('The configuration has been commited to the current site phase.'));
  $form_state['redirect'] = 'admin/config/system/site-phases';
}

/**
 * Confirmation form for configuration commit on the current site phase.
 */
function site_phase_configure_abort_confirm($form, &$form_state) {
  // @todo: would be nice to have a list of changes here.
  $form['confirmation'] = array(
    '#type' => 'checkbox',
    '#required' => TRUE,
    '#title' => t('Yes, I want to abort the changes I made on the current phase.'),
  );
  return confirm_form($form, t('Are you sure you want to abort the changes on the current phase?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to abort the changes on the current phase?'));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_configure_abort_confirm_submit($form, &$form_state) {
  site_phase_abort_configuration();
  drupal_set_message(t('The configuration has been aborted on the current site phase.'));
  $form_state['redirect'] = 'admin/config/system/site-phases';
}

/**
 * Confirmation form for configuration commit on the current site phase.
 */
function site_phase_delete_confirm($form, &$form_state, $phase) {
  $form['phase'] = array(
    '#type' => 'value',
    '#value' => $phase->machine_name,
  );
  return confirm_form($form, t('Are you sure you want to delete the site phase?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to delete the site phase?'));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_delete_confirm_submit($form, &$form_state) {
  site_phase_delete($form_state['values']['phase']);
  drupal_set_message(t('The site phase has been deleted.'));
  $form_state['redirect'] = 'admin/config/system/site-phases';
}

/**
 * Confirmation form for configuring a site phase.
 */
function site_phase_save_configuration_confirm($form, &$form_state, $phase) {
  $form['phase_name'] = array(
    '#type' => 'value',
    '#value' => $phase->machine_name,
  );
  $form['confirmation'] = array(
    '#type' => 'checkbox',
    '#required' => TRUE,
    '#title' => t('Yes, I want to save the current configuration into the %phase_name phase and I understand the consequences of this action.', array('%phase_name' => $phase->name)),
  );
  return confirm_form($form, t('Are you sure you want to overwrite the phase configuration?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to overwrite the %phase_name site phase configuation?', array('%phase_name' => $phase->name)));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_save_configuration_confirm_submit($form, &$form_state) {
  site_phase_commit_configuration($form_state['values']['phase_name']);
  drupal_set_message(t('The configuration has been saved.'));
  $form_state['redirect'] = 'admin/config/system/site-phases';
}
