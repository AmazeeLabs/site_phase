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
    $actions[] = l(t('configure'), site_phase_get_configure_path($phase->spid), array('query' => $destination));
    $actions[] = l(t('activate'), site_phase_get_activate_path($phase->spid), array('query' => $destination));
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
 * Confirmation form for configuration commit on the current site phase.
 */
function site_phase_copy_current_site_state_confirm($form, &$form_state, $phase, $plugin, $configuration_area) {
  $form['phase'] = array(
    '#type' => 'value',
    '#value' => $phase->machine_name,
  );
  $form['plugin'] = array(
    '#type' => 'value',
    '#value' => $plugin->getId(),
  );
  $form['configuration_area'] = array(
    '#type' => 'value',
    '#value' => $configuration_area,
  );
  $configuration_areas = $plugin->getConfigurationAreas();
  return confirm_form($form, t('Are you sure you want to overwrite the configuration of the phase?'), 'admin/config/system/site-phases',
                      t('Are you sure you want to overwrite the %configuration_area of the %plugin_name for the %phase_name phase with the current state of the site?',
                          array('%configuration_area' => $configuration_areas[$configuration_area]['title'], '%plugin_name' => $plugin->getTitle(),  '%phase_name' => $phase->name)));
}

/**
 * Submit handler for the activate phase confirmation form.
 */
function site_phase_copy_current_site_state_confirm_submit($form, &$form_state) {
  site_phase_copy_current_state_to_phase($form_state['values']['phase'], $form_state['values']['plugin'], $form_state['values']['configuration_area']);
  $phase = site_phase_machine_name_load($form_state['values']['phase']);
  $plugin = site_phase_plugin_load($form_state['values']['plugin']);
  $configuration_areas = $plugin->getConfigurationAreas();
  drupal_set_message(t('The current state of the site for the %plugin_name:%configuration_area has been copied to the %site_phase phase.',
                      array( '%site_phase' => $phase->name, '%plugin_name' => $plugin->getTitle(), '%configuration_area' => $configuration_areas[$form_state['values']['configuration_area']]['title'])));
  $form_state['redirect'] = 'admin/config/system/site-phases';
}

/**
 * Menu callback for the phase configuration page.
 */
function site_phase_configure_page($phase) {
  $plugins = site_phase_get_all_configuration_plugins();
  $links = array();
  foreach ($plugins as $key => $plugin) {
    $plugin_object = site_phase_plugin_load($key);
    $links[$key]['data'] = $plugin_object->getTitle();

    $configuration_areas = $plugin_object->getConfigurationAreas();
    $plugin_id = $plugin_object->getId();
    foreach ($configuration_areas as $configuration_key => $configuration_area) {
      $links[$key]['children'][] = l($configuration_area['title'], site_phase_get_configure_path($phase->spid) . '/' . $plugin_id . '/' . $configuration_key, array('query' => drupal_get_destination()));
    }
  }
  return array(
    'title' => array(
      '#markup' => '<h2>' . t('Configure phase %site_phase', array('%site_phase' => $phase->name)) . '</h2>',
    ),
    'links' => array(
      '#theme' => 'item_list',
      '#items' => $links,
    ),
  );
}

/**
 * Form constructor for configuring the plugin of a site phase.
 */
function site_phase_plugin_configure_form($form, &$form_state, $phase, $plugin, $configuration_area) {
  $form['phase'] = array(
    '#type' => 'value',
    '#value' => $phase->machine_name,
  );
  $form['plugin'] = array(
    '#type' => 'value',
    '#value' => $plugin->getId(),
  );
  $form['configuration_area'] = array(
    '#type' => 'value',
    '#value' => $configuration_area,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 50,
  );
  $form['copy_current_state'] = array(
    '#type' => 'submit',
    '#value' => t('Copy current site state'),
    '#weight' => 51,
    '#submit' => array('site_phase_plugin_configure_copy_state_submit'),
  );
  $configuration = site_phase_configuration_load($phase->machine_name, $plugin->getId());
  $default_phase = site_phase_default_load();
  $default_configuration = site_phase_configuration_load($default_phase->machine_name, $plugin->getId());
  $plugin->configurationForm($form, $form_state, $configuration_area, $default_configuration, $configuration);
  return $form;
}

/**
 * Submit handler for configuring the plugin of a phase.
 */
function site_phase_plugin_configure_form_submit(&$form, &$form_state) {
  $plugin = site_phase_plugin_load($form_state['values']['plugin']);
  $plugin_state = $plugin->getConfigurationFromSubmission($form_state['values']['configuration_area'], $form, $form_state);
  site_phase_configuration_save($form_state['values']['phase'], $form_state['values']['plugin'], $form_state['values']['configuration_area'], $plugin_state);
  drupal_set_message(t('The configurations have been saved.'));
}

function site_phase_plugin_configure_copy_state_submit(&$form, &$form_state) {
  $phase = site_phase_machine_name_load($form_state['values']['phase']);
  $options = array();
  if (!empty($_GET['destination'])) {
    $options['query']['destination'] = $_GET['destination'];
    unset($_GET['destination']);
  }
  $form_state['redirect'] = array(site_phase_get_configure_path($phase->spid) . '/' . $form_state['values']['plugin'] . '/' . $form_state['values']['configuration_area'] . '/copy-current-state', $options);
}
