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
    $permissions_modules = user_permission_get_modules();
    // For each available permission, if the permission is granted for a role,
    // then we saved it in the database, otherwise we delete it from the
    // database.
    $transaction = db_transaction();
    try {
      if (!empty($configuration['available_permissions']->configuration['permissions']) && !empty($configuration['permissions']->configuration['permissions'])) {
        // Instead of deleting one permission at a time, we make a batch
        // deletion. Basically, we delete all the permissions which are
        // available for configuration (for the set of roles which we have
        // the configuration set). The permissions will later be added if needed.
        $rids = array_keys($configuration['permissions']->configuration['permissions']);
        $available_permissions = array_chunk($configuration['available_permissions']->configuration['permissions'], 50, TRUE);
        foreach ($available_permissions as $perms) {
          db_delete('role_permission')->condition('rid', $rids)->condition('permission', $perms)->execute();
        }

        // Time to add the permissions based on the configuration.
        $query = db_insert('role_permission')->fields(array('rid', 'permission', 'module'));
        $execute_query = FALSE;
        foreach ($configuration['available_permissions']->configuration['permissions'] as $permission_key) {
          foreach ($configuration['permissions']->configuration['permissions'] as $rid => $permissions) {
            if (!empty($permissions[$permission_key]) && !empty($permissions_modules[$permission_key])) {
              $query->values(array($rid, $permission_key, $permissions_modules[$permission_key]));
              $execute_query = TRUE;
            }
          }
        }
        if ($execute_query) {
          $query->execute();
        }
      }
    }
    catch (Exception $e) {
      $transaction->rollback();
    }
  }

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::mergeConfiguration()
   */
  public function mergeConfiguration($conf1, $conf2) {
    // For all the available permissions defined in $conf2, we overwrite the
    // settings in $conf1.
    $result = $conf1;
    if (!empty($conf2['available_permissions']->configuration['permissions'])) {
      foreach ($conf2['available_permissions']->configuration['permissions'] as $permission_string) {
        if (!empty($conf2['permissions'])) {
          foreach ($conf2['permissions']->configuration['permissions'] as $rid => $permissions) {
            if (empty($permissions[$permission_string]) && !empty($result['permissions']->configuration['permissions'][$rid][$permission_string])) {
              unset($result['permissions']->configuration['permissions'][$rid][$permission_string]);
            } elseif (!empty($permissions[$permission_string])) {
              $result['permissions']->configuration['permissions'][$rid][$permission_string] = $permission_string;
              // Also, in this case, make sure the the results has the permission
              // string in the available permissions.
              $result['available_permissions']->configuration['permissions'][$permission_string] = $permission_string;
            }
          }
        }
      }
    }
    return $result;
  }

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::getCurrentSiteConfiguration()
   */
  public function getCurrentSiteConfiguration() {
    $configuration = array('available_permissions' => array(), 'permissions' => array());
    // For the available permissions, we basically return all the permissions
    // from the system.
    $all_permissions = module_invoke_all('permission');
    foreach ($all_permissions as $perm_key => $permission) {
      $configuration['available_permissions']['permissions'][$perm_key] = $perm_key;
    }
    // For the permission configuration, we take all the data from the
    // role_permission table.
    $query = db_select('role_permission', 'rp')->fields('rp', array('rid', 'permission'));
    $result = $query->execute()->fetchAll();
    foreach ($result as $row) {
      $configuration['permissions']['permissions'][$row->rid][$row->permission] = $row->permission;
    }
    return $configuration;
  }

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::configurationForm()
   */
  public function configurationForm(&$form, &$form_state, $phase, $configuration_area, $default_phase_configuration = NULL, $configuration = NULL) {
    switch ($configuration_area) {
      case 'available_permissions': {
        $this->configurationAvailablePermissionsForm($form, $form_state, $phase, $default_phase_configuration, $configuration);
        break;
      }
      case 'permissions': {
        $this->configurationPermissionsForm($form, $form_state, $phase, $default_phase_configuration, $configuration);
        break;
      }
    }
  }

  /**
   * Form constructor for the available permissions configuration area.
   */
  protected function configurationAvailablePermissionsForm(&$form, &$form_state, $phase, $default_phase_configuration = NULL, $configuration = NULL) {
    // For the default phase, this form is not really usefull.
    if ($phase->is_default) {
      $form['not_available'] = array('#markup' => t('This configuration is not available for the default phase. It will always enable all the permissions for edit.'));
      $form['submit']['#access'] = $form['copy_current_state']['#access'] = FALSE;
      return;
    }
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
          $options[$module_info[$module]['name']][$perm_key] = filter_xss($permission['title'], array());
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
  protected function configurationPermissionsForm(&$form, &$form_state, $phase, $default_phase_configuration = NULL, $configuration = NULL) {
    // If we have no 'roles' in the request, then we show a roles selection
    // form.
    // This is a temporary solution for when there are many roles and
    // permissions in the system which will block the page.
    if (empty($_GET['roles'])) {
      $form['roles'] = array(
        '#type' => 'select',
        '#title' => t('Roles'),
        '#multiple' => TRUE,
        '#options' => array('all' => t('All')) + user_roles(),
        '#size' => 10,
        '#required' => TRUE,
      );
      $form['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
      );
      unset($form['copy_current_state'], $form['submit']);
    }
    else {
      if (empty($configuration['permissions']) && !empty($default_phase_configuration['permissions'])) {
        $configuration['permissions'] = $default_phase_configuration['permissions'];
      }
      module_load_include('inc', 'user', 'user.admin');
      $form = user_admin_permissions($form, $form_state);
      // The user_admin_permissions can return the form for either all of the
      // roles, or only for one single role. We think it is really better to
      // have the possibility to edit multiple roles, so we get all of them and
      // afterwards we unset the checkboxes for the ones we do not need.
      if (empty($_GET['roles']['all'])) {
        foreach (user_roles() as $rid => $role_name) {
          if (empty($_GET['roles'][$rid])) {
            unset($form['checkboxes'][$rid], $form['role_names'][$rid], $form['role_names']['#value'][$rid]);
          }
        }
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
  }

  /**
   * (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::getConfigurationFromSubmission()
   */
  public function getConfigurationFromSubmission($phase, $configuration_area, &$form, &$form_state) {
    $configuration = array();
    switch ($configuration_area) {
      case 'available_permissions': {
        $configuration['permissions'] = $form_state['values']['available_permissions'];
        break;
      }
      case 'permissions': {
        // If we submitted the roles, then just redirect to the same page but
        // with the roles parameter in the request.
        if (!empty($form_state['values']['roles'])) {
          $query = drupal_get_query_parameters();
          $query['roles'] = $form_state['values']['roles'];
          return array('redirect' => array($_GET['q'], array('query' => $query)));
        }
        $current_phase_configuration = site_phase_configuration_load($phase->machine_name, $this->getId(), $configuration_area);
        foreach (user_roles() as $rid => $name) {
          // If we submitted the permissions for this role, we use them,
          // otherwise we will use the current configuration of the phase. If we
          // do not do that, the configuration for the roles which were not on
          // the form will be lost.
          if (isset($form_state['values']['role_names'][$rid])) {
            $configuration['permissions'][$rid] = array_filter($form_state['values'][$rid]);
          }
          else {
            $configuration['permissions'][$rid] = $current_phase_configuration->configuration[$configuration_area][$rid];
          }
        }
        break;
      }
    }
    return $configuration;
  }
}
