<?php

/**
 * @file
 *  Site state plugin for the user permissions.
 */

/**
 * Site state plugin class for the core permissions.
 */
class SiteStateUserPermPlugin extends SiteStateBasePlugin {

  /**
   * (non-PHPdoc)
   * @see SiteStatePluginInterface::applyState()
   */
  public function applyState($state) {
    // When we apply a state, we basically replace the entire role_permission
    // table.
    $transaction = db_transaction();
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
    }
  }

  /**
   * (non-PHPdoc)
   * @see SiteStatePluginInterface::configurationForm()
   */
  public function configurationForm($state = NULL) {
    module_load_include('inc', 'user', 'user.admin');
    $form = array();
    $form_state = array();
    $form = user_admin_permissions($form, $form_state);
    // Hack for the login toboggan: the module will alter the user.permissions.js
    // but if logintoboggan_form_user_admin_permissions_alter() is not called,
    // it will generated an error in javascript.
    if (function_exists('logintoboggan_form_user_admin_permissions_alter')) {
      logintoboggan_form_user_admin_permissions_alter($form, $form_state);
    }
    unset($form['actions']);
    $form['#theme'] = 'user_admin_permissions';

    // Set the default value according to the data from the state.
    foreach ($form['checkboxes'] as $rid => &$chekbox) {
      // If we have absolutely no information for a role in the state, then we
      // can just skip it.
      if (!isset($state->state[$rid])) {
        continue;
      }
      $form['checkboxes'][$rid]['#default_value'] = $state->state[$rid];
    }
    return $form;
  }

  /**
   * (non-PHPdoc)
   * @see SiteStatePluginInterface::getStateFromSubmission()
   */
  public function getStateFromSubmission(&$form, &$form_state) {
    $state = array();
    foreach ($form_state['values']['role_names'] as $rid => $name) {
      $state[$rid] = array_filter($form_state['values'][$rid]);
    }
    return $state;
  }
}
