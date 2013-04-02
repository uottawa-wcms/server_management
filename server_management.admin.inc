<?php

function server_management_config_list_types() {
  $types = server_management_config_types();
  $content = array();
  foreach ($types as $type) {
    $content[] = array(
      'title' => $type->label,
      'href' => 'admin/structure/server_config/manage/' . $type->type,
      'description' => !empty($type->description) ? $type->description : '',
      'localized_options' => array(),
    );
  }
  return theme('admin_block_content', array('content' => $content));
}

function server_management_config_type_form($form, &$form_state, $config_type = NULL) {
  if (empty($config_type)) {
    $config_type = (object) array(
      'label' => '',
      'type' => '',
      'cardinality' => 0,
    );
  } else {
    $types = server_management_config_types();
    $config_type = $types[$config_type];
  }
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#maxlength' => 255,
    '#default_value' => $config_type->label,
  );
  if (empty($config_type->type)) {
    $form['type'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine Name'),
      '#required' => TRUE,
      '#maxlength' => 32,
      '#machine_name' => array(
        'exists' => 'server_management_config_type_name_exists',
        'source' => array('label')
      ),
    );
  } else {
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $config_type->type,
    );
  }
  $form['cardinality'] = array(
    '#type' => 'textfield',
    '#title' => t('Cardinality'),
    '#description' => t('The maximum number of instances a single server can have of this type. Enter 0 for unlimited.'),
    '#required' => TRUE,
    '#default_value' => $config_type->cardinality,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function server_management_config_type_form_validate($form, &$form_state) {
  if (!filter_var($form_state['values']['cardinality'], FILTER_VALIDATE_INT)) {
    form_set_error('cardinality', t('You must provide an integer value for cardinality'));
  }
}

function server_management_config_type_form_submit($form, &$form_state) {
  $type = array(
    'label' => $form_state['values']['label'],
    'type' => $form_state['values']['type'],
    'cardinality' => $form_state['values']['cardinality'],
  );
  db_merge('server_management_config_types')
      ->fields($type)
      ->key(array('type' => $type['type']))
      ->execute();
  drupal_set_message(t('Server configuration packet type updated'));
  $form_state['redirect'] = 'admin/structure/server_config';
}

function server_management_server_form($form, &$form_state) {
  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . t('There are no configuration settings for servers at
      this time') . '</p>',
  );
}

