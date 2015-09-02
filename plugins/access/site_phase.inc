<?php

/**
 * @file
 *  Plugin to provide access based on the site phase.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Site phase"),
  'description' => t('Control access based on the site phase.'),
  'callback' => 'site_phase_ctools_access_check',
  'settings form' => 'site_phase_ctools_access_settings',
  'summary' => 'site_phase_ctools_access_summary',
);

/**
 * Settings form for the site phase access plugin.
 */
function site_phase_ctools_access_settings(&$form, &$form_state, $conf) {
  $site_phases = site_phase_get_all_phases();
  $options = array();
  foreach ($site_phases as $phase) {
    $options[$phase->machine_name] = $phase->name;
  }
  $form['settings']['site_phase'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Grant access if the site phase is one of'),
    '#options' => $options,
    '#default_value' => $conf['site_phase'],
  );
  return $form;
}

/**
 * Check for access.
 */
function site_phase_ctools_access_check($conf, $context) {
  $current_site_phase = site_phase_get_current_phase();
  if ($conf['site_phase'][$current_site_phase] === $current_site_phase) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Provides the summary.
 */
function site_phase_ctools_access_summary($conf, $context) {
  $site_phases = site_phase_get_all_phases();
  $phases = array();
  foreach ($site_phases as $phase) {
    if (!empty($conf['site_phase'][$phase->machine_name])) {
      $phases[] = $phase->name;
    }
  }

  return t('Site phase must be one of: !site_phases', array('!site_phases' => implode(', ', $phases)));
}
