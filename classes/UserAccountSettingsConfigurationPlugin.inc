<?php

/**
 * @file
 * Contains \UserAccountSettingsConfigurationPlugin.
 */

/**
 * Site configuration plugin class for the user account settings.
 */
class UserAccountSettingsConfigurationPlugin extends SiteConfigurationBasePlugin {

  /**
   * {@inheritdoc}
   */
  public function applyConfiguration($configuration) {
    if (isset($configuration['account_settings']->configuration['account_settings']['user_register'])) {
      variable_set('user_register', $configuration['account_settings']->configuration['account_settings']['user_register']);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function mergeConfiguration($conf1, $conf2) {
    if (isset($conf2['account_settings']->configuration['account_settings']['user_register'])) {
      $conf1['account_settings']->configuration['account_settings']['user_register'] = $conf2['account_settings']->configuration['account_settings']['user_register'];
    }
    return $conf1;
  }

  /**
   * {@inheritdoc}
   */
  public function getCurrentSiteConfiguration() {
    return array(
      'account_settings' => array(
        'account_settings' => array(
          'user_register' => variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL),
        ),
      ),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function configurationForm(&$form, &$form_state, $phase, $configuration_area, $default_phase_configuration = NULL, $configuration = NULL) {
    $form['account_settings']['#tree'] = TRUE;
    $form['account_settings']['user_register'] = array(
      '#type' => 'radios',
      '#title' => t('Who can register accounts?'),
      '#options' => array(
        USER_REGISTER_ADMINISTRATORS_ONLY => t('Administrators only'),
        USER_REGISTER_VISITORS => t('Visitors'),
        USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL => t('Visitors, but administrator approval is required'),
      ),
      '#default_value' => isset($configuration['account_settings']->configuration['account_settings']['user_register'])
        ? $configuration['account_settings']->configuration['account_settings']['user_register']
        : NULL,
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getConfigurationFromSubmission($phase, $configuration_area, &$form, &$form_state) {
    return array(
      'account_settings' => $form_state['values']['account_settings'],
    );
  }

}
