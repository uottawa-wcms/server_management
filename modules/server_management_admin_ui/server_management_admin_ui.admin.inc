<?php

function server_management_admin_ui_server_load_by_parent($parent_sid = NULL) {
  $efq = new EntityFieldquery();
  $efq->entityCondition('entity_type', 'server');
  if (empty($parent_sid)) {
    $efq->propertyCondition('parent_sid', 1, '<');
  } else {
    $efq->propertycondition('parent_sid', $parent_sid);
  }
  $results = array();
  $rs = $efq->execute();
  if (empty($rs['server'])) {
    return array();
  }
  foreach (array_keys($rs['server']) as $sid) {
    $results[$sid] = server_management_load($sid);
  }
  return $results;
}

function _server_management_admin_ui_build_option_list() {
  $rs = db_query('SELECT sid, parent_sid, label FROM {server_management_servers} ORDER BY parent_sid ASC, label ASC');
  $basic_options = array();
  foreach ($rs as $row) {
    if (empty($row->parent_sid)) {
      $row->parent_sid = 0;
    }
    if (empty($basic_options[$row->parent_sid])) {
      $basic_options[$row->parent_sid] = array();
    }
    $basic_options[$row->parent_sid][$row->sid] = $row->label;
  }
  $nested_options = array();
  if (!empty($basic_options[0])) {
    $nested_options = _server_management_option_list_recurse($basic_options, 0, 0);
  }
  return $nested_options;
}

function _server_management_admin_ui_option_list_recurse(&$basic_options, $parent_id, $depth) {
  $options = array();
  $label_prefix = '';
  for ($k = 0; $k < $depth; $k++) {
    $label_prefix .= '-';
  }
  $label_prefix .= ' ';
  if (isset($basic_options[$parent_id])) {
    foreach ($basic_options[$parent_id] as $sid => $label) {
      $options[$sid] = $label_prefix . $label;
      $options += _server_management_option_list_recurse($basic_options, $sid, $depth + 1);
    }
  }
  return $options;
}

function server_management_admin_ui_server_list() {
  $header = array(
    'label' => t('Server Name'),
    'state' => t('State'),
    'actions' => t('Actions'),
  );
  $rows = _server_management_server_list();
  return theme('table', array(
    'rows' => $rows,
    'header' => $header,
    'empty' => t('No servers have been defined yet'),
  ));
}

function _server_management_admin_ui_server_list($parent = NULL, $depth = 0) {
  $rows = array();
  $servers = server_management_server_load_by_parent($parent);
  foreach ($servers as $server) {
    $rows[$server->sid] = _server_management_create_row($server, $depth);
    $rows += _server_management_server_list($server->sid, $depth + 1);
  }
  return $rows;
}

function _server_management_admin_ui_create_row($server, $depth = 0) {
  $margin = 0;
  for ($k = 0; $k < $depth; $k++) {
    $margin += 15;
  }
  return array(
    'label' => '<span style="padding-left: ' . $margin . 'px">' .
        l($server->label, 'admin/data/servers/' . $server->sid) . '</span>',
    'state' => $server->status ? t('Active') : t('Disabled'),
    'actions' => implode(' ', array(
      l('edit', 'admin/data/servers/' . $server->sid . '/edit'),
      l('delete', 'admin/data/servers/' . $server->sid . '/delete'),
    )),
  );
}

function server_management_admin_ui_delete_form($form, &$form_state, $server) {

}

function server_management_admin_ui_edit_form($form, &$form_state, $server = NULL) {
  if (empty($server)) {
    $server = (object) array(
      'label' => '',
      'status' => 0,
      'parent_sid' => 0,
      'sid' => NULL,
      'access_key' => '',
      'machine_name' => '',
    );
  }
  $form_state['original'] = $server;
  $form['label'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Label'),
    '#description' => t('Human readable name of the server'),
    '#default_value' => $server->label,
  );
  if (empty($server->machine_name)) {
    $form['machine_name'] = array(
      '#type' => 'machine_name',
      '#default_value' => '',
      '#maxlength' => 32,
      '#machine_name' => array(
        'exists' => 'server_management_name_exists',
        'source' => array('label')
      ),
    );
  }
  $form['access_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Key'),
    '#description' => t('Access key for the server to load its own profile data. Leaving this blank will automatically generate a new one.'),
    '#default_value' => $server->access_key,
  );
  $form['parent_sid'] = array(
    '#type' => 'select',
    '#title' => t('Parent Server'),
    '#options' => array(
      0 => 'none',
    ) + _server_management_build_option_list(),
    '#default_value' => $server->parent_sid,
  );
  $form['status'] = array(
    '#type' => 'select',
    '#options' => array(
      0 => 'Disabled',
      1 => 'Active',
    ),
    '#required' => TRUE,
    '#title' => t('Status'),
    '#default_value' => $server->status,
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 1000,
  );
  field_attach_form('server', $server, $form, $form_state);
  return $form;
}

function server_management_admin_ui_edit_form_validate($form, &$form_state) {
  field_attach_validate('server', $form_state['original'], $form, $form_state);
}

function server_management_admin_ui_edit_form_submit($form, &$form_state) {
  $server = $form_state['original'];
  $server->label = $form_state['values']['label'];
  if (empty($server->machine_name)) {
    $server->machine_name = $form_state['values']['machine_name'];
  }
  $server->access_key = $form_state['values']['access_key'];
  $server->parent_sid = $form_state['values']['parent_sid'];
  $server->status = $form_state['values']['status'];
  field_attach_submit('server', $server, $form, $form_state);
  server_management_save_server($server);
  drupal_set_message(t('Server saved successfully'));
  $form_state['redirect'] = 'admin/data/servers/' . $server->sid;
}



function server_management_admin_ui_view($server) {
  $controller = entity_get_controller('server');
  return $controller->view(array($server->sid => $server));
}

function server_management_admin_ui_config_item_load($server_id) {
  $q = db_select('server_management_config', 'smc');
  $q->fields('smc');
  $q->condition('sid', $server_id);
  return $q->execute()->fetchAll();
}

function server_management_admin_ui_list_config_for_server($server) {
  $packets = server_management_config_item_load($server->sid);
  $header = array(
    'label' => t('Label'),
    'type' => t('Type'),
    'actions' => t('Actions'),
  );
  $rows = array();
  foreach ($packets as $packet) {
    $rows[] = array(
      'label' => l($packet->label, 'admin/data/servers/' . $server->sid . '/config/' . $packet->cid),
      'type' => $packet->type,
      'actions' => implode(' ', array(
        l(t('edit'), 'admin/data/servers/' . $server->sid . '/config/' . $packet->cid . '/edit'),
        l(t('delete'), 'admin/data/servers/' . $server->sid . '/config/' . $packet->cid . '/delete'),
      )),
    );
  }
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('There are no configuration items for this server'),
  ));
}

function server_management_admin_ui_config_item_type_for_add($server) {
  $types = server_management_config_types();
  $content = array();
  foreach ($types as $type) {
    // @todo check $server to see if it's met it's cardinality limit for $type
    $content[] = array(
      'title' => $type->label,
      'href' => 'admin/data/servers/' . $server->sid . '/config/add/' . $type->type,
      'description' => !empty($type->description) ? $type->description : '',
      'localized_options' => array(),
    );
  }
  if (count($content)) {
    return theme('admin_block_content', array('content' => $content));
  } else {
    return '<p>' . t('There are no server configuration packet types yet.') . '</p>';
  }
}

function server_management_admin_ui_config_item_form($form, &$form_state, $server, $item) {
    if (!is_object($item)) {
    $item = (object) array(
      'type' => $item,
      'label' => '',
      'inherit' => 0,
      'sid' => $server->sid,
      'cid' => NULL,
      'uuid' => NULL,
    );
  }
  $form['label'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Label'),
    '#default_value' => $item->label,
  );
  $form['inherit'] = array(
    '#type' => 'select',
    '#options' => array(
      0 => t('none'),
      1 => t('inherit'),
    ),
    '#title' => t('Inheritance'),
    '#description' => t('Will subsites inherit this configuration setting?'),
  );
  field_attach_form('server_config', $item, $form, $form_state);
  $form_state['original'] = $item;
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 50,
  );
  return $form;
}

function server_management_admin_ui_config_item_form_validate($form, &$form_state) {
  field_attach_form_validate('server_config', $form_state['original'], $form, $form_state);
}

function server_management_admin_ui_config_item_form_submit($form, &$form_state) {
  $controller = entity_get_controller('server_config');
  $object = $form_state['original'];
  $object->label = $form_state['values']['label'];
  $object->inherit = $form_state['values']['inherit'];
  field_attach_submit('server_config', $object, $form, $form_state);
  $controller->save($object);
  drupal_set_message(t('Configuration saved'));
  $form_state['redirect'] = 'admin/data/servers/' . $object->sid;
}

function server_management_admin_ui_config_item_delete_form($form, $form_state, $item) {

}

function server_management_admin_ui_view_config_item($item) {
  $controller = entity_get_controller('server_config');
  return $controller->view(array($item->cid => $item));
}