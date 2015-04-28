<?php

/**
 * @file
 *  Site configuration plugin for the user permissions.
 */

/**
 * Site configuration plugin class for the core permissions.
 */
class SiteUserPermConfigurationPlugin extends SiteConfigurationBasePlugin {

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::applyConfiguration()
   */
  public function applyConfiguration($configuration) {
    // @todo: This need to be completely rewritten.
    /*$transaction = db_transaction();
    try {
      // We must have something in the state, otherwise do nothing.
      if (!empty($state)) {
        db_truncate('role_permission')->execute();
        $query = db_insert('role_permission')->fields(array('rid', 'permission', 'module'));
        foreach ($state as $permission) {
          $query->values(array($permission->rid, $permission->permission, $permission->module));
        }
        $query->execute();
      }
    }
    catch (Exception $e) {
      $transaction->rollback();
    }*/
  }

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::configurationForm()
   */
  public function configurationForm(&$form, &$form_state, $configuration_area, $configuration = NULL) {
    switch ($configuration_area) {
      case 'available_permissions': {
        $this->configurationAvailablePermissionsForm($form, $form_state, $configuration);
        break;
      }
      case 'permissions': {
        $this->configurationPermissionsForm($form, $form_state, $configuration);
        break;
      }
    }
  }

  /**
   * Form constructor for the available permissions configuration area.
   */
  protected function configurationAvailablePermissionsForm(&$form, &$form_state, $configuration = NULL) {
    $modules = array();
    $module_info = system_get_info('module');
    foreach (module_implements('permission') as $module) {
      $modules[$module] = $module_info[$module]['name'];
    }
    asort($modules);

    $options = array();
    foreach ($modules as $module => $display_name) {
      if ($permissions = module_invoke($module, 'permission')) {
        foreach ($permissions as $perm_key => $permission) {
          $options[$module_info[$module]['name']][$perm_key] = $permission['title'];
        }
      }
    }

    $form['available_permissions'] = array(
      '#type' => 'select',
      '#title' => t('Available permissions to configure'),
      '#options' => $options,
      '#multiple' => TRUE,
      '#size' => 40,
      '#default_value' => !empty($configuration['available_permissions']->configuration['permissions']) ? $configuration['available_permissions']->configuration['permissions'] : array(),
    );
  }

  /**
   * Form constructor for the permissions configuration area.
   */
  protected function configurationPermissionsForm(&$form, &$form_state, $configuration = NULL) {
    module_load_include('inc', 'user', 'user.admin');
    $form = user_admin_permissions($form, $form_state);

    // Hack for the login toboggan: the module will alter the user.permissions.js
    // but if logintoboggan_form_user_admin_permissions_alter() is not called,
    // it will generated an error in javascript.
    if (function_exists('logintoboggan_form_user_admin_permissions_alter')) {
      logintoboggan_form_user_admin_permissions_alter($form, $form_state);
    }
    unset($form['actions']);
    $form['#theme'] = 'user_admin_permissions';

    // Remove the permissions which are not available for editing.
    $previous_numeric = NULL;
    $all_removed = TRUE;
    foreach ($form['permission'] as $perm_key => $permission) {
      if (is_numeric($perm_key)) {
        // If we removed all the entries from the previous numeric key, then we
        // remove also the numeric key.
        if (isset($previous_numeric) && !empty($all_removed)) {
          unset($form['permission'][$previous_numeric]);
        }
        $previous_numeric = $perm_key;
        $all_removed = TRUE;
        continue;
      }
      if (empty($configuration['available_permissions']->configuration['permissions'][$perm_key])) {
        unset($form['permission'][$perm_key]);
        foreach ($form['checkboxes'] as $rid => &$chekbox) {
          unset($form['checkboxes'][$rid]['#options'][$perm_key]);
        }
      }
      else {
        $all_removed = FALSE;
      }
    }
    // At the end, we may have a special case, when the last module should also
    // be removed.
    if (!empty($form['permission'])) {
      end($form['permission']);
      $last_key = key($form['permission']);
      if (is_numeric($last_key)) {
        unset($form['permission'][$last_key]);
      }
    }

    // Set the default value according to the data from the configuration.
    foreach ($form['checkboxes'] as $rid => &$chekbox) {
      // If we have absolutely no information for a role in the configuration,
      // then we can just skip it.
      if (!isset($configuration['permissions']->configuration['permissions'][$rid])) {
        continue;
      }
      $form['checkboxes'][$rid]['#default_value'] = $configuration['permissions']->configuration['permissions'][$rid];
    }
  }

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::getConfigurationFromSubmission()
   */
  public function getConfigurationFromSubmission($configuration_area, &$form, &$form_state) {
    $configuration = array();
    switch ($configuration_area) {
      case 'available_permissions': {
        $configuration['permissions'] = $form_state['values']['available_permissions'];
        break;
      }
      case 'permissions': {
        foreach ($form_state['values']['role_names'] as $rid => $name) {
          $configuration['permissions'][$rid] = array_filter($form_state['values'][$rid]);
        }
        break;
      }
    }
    return $configuration;
  }
}
