<?php

/**
 * @file
 *  Site state plugin for the user permissions.
 */

/**
 * Site state plugin class for the core permissions.
 */
class SiteStateUserPermPlugin implements SiteStatePluginInterface {

  public function retrieveCurrentState() {
    // We retrieve the entire role_permission table.
    $query = db_select('role_permission', 'rp')->fields('rp');
    return $query->execute()->fetchAll();
  }

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
}
