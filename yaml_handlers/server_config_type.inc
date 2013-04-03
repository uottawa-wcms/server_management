<?php

function yamlconfig_server_config_type_handler_preprocess(&$def) {
  $def += array(
    'cardinality' => 0,
  );
}

function yamlconfig_server_config_type_handler_validate($def) {
  if (empty($def['type'])) {
    return FALSE;
  }
  if (empty($def['label'])) {
    return FALSE;
  }
  return TRUE;
}

function yamlconfig_server_config_type_handler_create($def) {
  server_management_config_type_save($def);
}

function yamlconfig_server_config_type_handler_refresh($def) {
  yamlconfig_server_config_type_handler_create($def);
}

function yamlconfig_server_config_type_handler_uninstall($def) {
  server_management_config_type_delete($def['type']);
}