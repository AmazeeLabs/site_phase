<?php

/**
 * @file
 *  Site configuration plugin for the OG permissions on the site.
 */

class SiteOGPermConfigurationPlugin extends SiteConfigurationBasePlugin {

	/** (non-PHPdoc)
   * @see SiteConfigurationPluginInterface::applyConfiguration()
   */
  public function applyConfiguration($configuration) {
    // When we apply a state, we basically replace the entire role_permission
    // table.
    // @todo: this need to be completeley rewritten.
    /*$transaction = db_transaction();
    try {
      // We must have something in the state, otherwise do nothing.
      if (!empty($configuration)) {
        db_truncate('og_role_permission')->execute();
        $query = db_insert('og_role_permission')->fields(array('rid', 'permission', 'module'));
        foreach ($configuration as $permission) {
          $query->values(array($permission->rid, $permission->permission, $permission->module));
        }
        $query->execute();
      }
    }
    catch (Exception $e) {
      $transaction->rollback();
    }*/
  }
/* (non-PHPdoc)
 * @see SiteConfigurationPluginInterface::configurationForm()
 */public function configurationForm($form, $form_state, $configuration_area, $state = NULL) {
    // TODO Auto-generated method stub
    }

/* (non-PHPdoc)
 * @see SiteConfigurationPluginInterface::getConfigurationFromSubmission()
 */public function getConfigurationFromSubmission($configuration_area, $form, $form_state) {
    // TODO Auto-generated method stub
    }

}
