<?php

/**
 * @file
 *  Site state plugin for the OG permissions on the site.
 */

class SiteStateOGPermPlugin implements SiteStatePluginInterface {

	/** (non-PHPdoc)
   * @see SiteStatePluginInterface::retrieveCurrentState()
   */
  public function retrieveCurrentState() {
    // We retrieve the entire og_role_permission table.
    $query = db_select('og_role_permission', 'orp')->fields('orp');
    return $query->execute()->fetchAll();
  }

	/** (non-PHPdoc)
   * @see SiteStatePluginInterface::applyState()
   */
  public function applyState($state) {
    // When we apply a state, we basically replace the entire role_permission
    // table.
    $transaction = db_transaction();
    try {
      // We must have something in the state, otherwise do nothing.
      if (!empty($state)) {
        db_truncate('og_role_permission')->execute();
        $query = db_insert('og_role_permission')->fields(array('rid', 'permission', 'module'));
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
