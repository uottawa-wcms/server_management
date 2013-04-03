<?php

function server_management_connect_config_form($form, &$form_state) {
  $settings = variable_get('server_management_connect', array(
    'central_link' => '',
    'machine_name' => '',
    'access_key' => '',
    'delay' => 1800,
    'enabled' => 0,
    'last_executed' => 0,
  ));
  $form['remote_server_link'] = array(
    '#title' => t('Central Address'),
    '#description' => t('Enter the URL to the central server configuration management server.'),
    '#type' => 'textfield',
    '#default_value' => $settings['central_link'],
  );
  $form['server_machine_name'] = array(
    '#title' => t('Server Name'),
    '#description' => t('Enter the machine name of your server configuration profile'),
    '#type' => 'textfield',
    '#default_value' => $settings['machine_name'],
  );
  $form['server_access_key'] = array(
    '#title' => t('Access Key'),
    '#description' => t('Enter the access key fo your server configuation profile.'),
    '#type' => 'textfield',
    '#default_value' => $settings['access_key'],
  );
  $form['server_connect_delay'] = array(
    '#title' => t('Update Frequency'),
    '#description' => t('Enter the minimum time (in seconds) between each attempt to get the configuration profile.'),
    '#type' => 'textfield',
    '#default_value' => $settings['delay'],
  );
  $form['server_connect'] = array(
    '#title' => t('Enable Server Connect'),
    '#description' => t('Enabling this will continuously synchronize your local configuration options with the remote server'),
    '#type' => 'checkbox',
    '#default_value' => !!$settings['enabled'],
  );
  $form['actions']['save_settings'] = array(
    '#value' => t('Save Settings'),
    '#type' => 'submit',
    '#submit' => array('server_management_connect_config_form_save'),
  );
  $form['actions']['test_connection'] = array(
    '#value' => t('Save Settings & Test Connection'),
    '#type' => 'submit',
    '#submit' => array('server_management_connect_config_form_test'),
  );
  $form['actions']['force_sync'] = array(
    '#value' => t('Save Settings & Force Sync'),
    '#type' => 'submit',
    '#submit' => array('server_management_connect_config_form_sync'),
  );
  return $form;
}

function server_management_connect_config_form_validate($form, &$form_state) {
  if ($form_state['values']['server_connect']) {
    if (empty($form_state['values']['remote_server_link'])) {
      form_set_error('remote_server_link', t('You must provide a central address if you are enabling server connect'));
    }
    if (empty($form_state['values']['server_machine_name'])) {
      form_set_error('remote_server_link', t('You must provide a central address if you are enabling server connect'));
    }
    if (empty($form_state['values']['server_access_key'])) {
      form_set_error('remote_server_link', t('You must provide a central address if you are enabling server connect'));
    }
    if (!filter_var($form_state['values']['remote_server_link'], FILTER_VALIDATE_URL)) {
      form_set_error('remote_server_link', t('The central address must be a valid URL'));
    }
  }
}

function server_management_connect_config_form_save($form, &$form_state) {
  $configuration = array(
    'central_link' => $form_state['values']['remote_server_link'],
    'machine_name' => $form_state['values']['server_machine_name'],
    'access_key' => $form_state['values']['server_access_key'],
    'delay' => $form_state['values']['server_connect_delay'],
    'enabled' => $form_state['values']['server_connect'],
  );
  variable_set('server_management_connect', $configuration);
  drupal_set_message(t('Server Connect settings saved'));
}

function server_management_connect_config_form_test($form, &$form_state) {
  server_management_connect_config_form_save($form, &$form_state);
  server_management_connect_process(SERVER_MANAGEMENT_CONNECT_TEST);
}

function server_management_connect_config_form_sync($form, &$form_state) {
  server_management_connect_config_form_save($form, &$form_state);
  server_management_connect_process(SERVER_MANAGEMENT_CONNECT_FORCE);

}