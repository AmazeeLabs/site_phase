<?php

/**
 * Integration with the Features module: expose the site phases as feature
 * components.
 */

/**
 * Implements hook_features_export_options().
 */
function site_phase_features_export_options() {
  $all_phases = site_phase_get_all_phases();
  $options = array();
  foreach ($all_phases as $site_phase) {
    $options[$site_phase->machine_name] = $site_phase->name;
  }
  return $options;
}

/**
 * Implements hook_features_export().
 */
function site_phase_features_export($data, &$export, $module_name = '') {
  // Add our module as a dependency.
  $export['dependencies']['site_phase'] = 'site_phase';
  foreach ($data as $site_phase) {
    $export['features']['site_phase'][$site_phase] = $site_phase;
  }
  return array();
}

/**
 * Implements hook_features_export_render().
 */
function site_phase_features_export_render($module, $data) {
  $code = array();
  $code[] = '  $site_phases = array();';
  $code[] = '';

  foreach ($data as $name) {
    if ($site_phase = site_phase_machine_name_load($name)) {
      unset($site_phase->spid);
      $identifier = features_var_export($name);
      $export = features_var_export($site_phase , '  ');
      $code[] = "  // Exported site phase: {$name}.";
      $code[] = "  \$site_phases[{$identifier}] = {$export};";
      $code[] = "";
    }
  }

  $code[] = '  return $site_phases;';
  $code = implode("\n", $code);
  return array('site_phase_feature_default_settings' => $code);
}

/**
 * Implements hook_features_revert().
 */
function site_phase_features_revert($module) {
  site_phase_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 */
function site_phase_features_rebuild($module) {
  if ($defaults = features_get_default('site_phase', $module)) {
    foreach ($defaults as $site_phase) {
      $site_phase = (object) $site_phase;
      if ($existing = site_phase_machine_name_load($site_phase->machine_name)) {
        $site_phase->spid = $existing->spid;
      }
      site_phase_save($site_phase);
    }
  }
}

// The following hooks are implemneted for the site_phase_configuration
// component.

/**
 * Implements hook_features_export_options().
 */
function site_phase_configuration_features_export_options() {
  $all_phases = site_phase_get_all_phases();
  $options = array();
  foreach ($all_phases as $site_phase) {
    $options[$site_phase->machine_name] = $site_phase->name;
  }
  return $options;
}

/**
 * Implements hook_features_export().
 */
function site_phase_configuration_features_export($data, &$export, $module_name = '') {
  // Add our module as a dependency.
  $export['dependencies']['site_phase'] = 'site_phase';
  $pipe = array();
  foreach ($data as $site_phase) {
    $export['features']['site_phase_configuration'][$site_phase] = $site_phase;
    // When we export the configuration of a phase, we must have that phase too.
    $pipe['site_phase'][] = $site_phase;
  }
  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function site_phase_configuration_features_export_render($module, $data) {
  $code = array();
  $code[] = '  $configuration = array();';
  $code[] = '';

  foreach ($data as $name) {
    if ($configuration = site_phase_configuration_load($name)) {
      $identifier = features_var_export($name);
      $export = features_var_export($configuration , '  ');
      $code[] = "  // Exported configuration for site phase: {$name}.";
      $code[] = "  \$configuration[{$identifier}] = {$export};";
      $code[] = "";
    }
  }

  $code[] = '  return $configuration;';
  $code = implode("\n", $code);
  return array('site_phase_configuration_feature_default_settings' => $code);
}

// @todo: Implement the revert and the rebuild hooks!