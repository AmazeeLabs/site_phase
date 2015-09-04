<?php

/**
 * Rules integration for the site_phase module
 */

function site_phase_rules_event_info() {
  return array(
    'site_phase_transition' => array(
      'label' => t("Site Phase is changed"),
      'group' => 'Site phase',
      'variables' => array(
        'from_phase' => array(
          'type' => 'site_phase',
          'label' => t("The phase from which the transition is made"),
        ),
        'to_phase' => array(
          'type' => 'site_phase',
          'label' => t("The phase to which the transition is made"),
        ),
      ),
    ),
  );
}
