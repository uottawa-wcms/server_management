<?php

function server_management_service_invoke_server_connect($remote_server, $machine_name, $access_key) {
  $service = new TwoWayService('server_management', 'server_connect', $remote_server);
  try {
    $results = $service->invoke(array(
      'machine_name' => $machine_name,
      'access_key' => $access_key,
    ));
    return $results;
  } catch (TwoWayException $e) {
    $e->dsm();
    $e->watchdog('server_management', 'An error occured while requesting a server connection');
    return array();
  }
}

function server_management_service_do_server_connect($args, &$message) {
  $remote_server = $message->getMessageVar('origin');
  $args = (array) $args;
  $server = server_management_load_by_name($args['machine_name']);
  if ($server->access_key == $args['access_key']) {
    _secsyn_fast_queue($remote_server, 'SecureSyndicationEntityProfile', $server, TRUE);
    return TRUE;
  }
  return FALSE;
}

function _server_management_queue_all_child_servers($remote, $server) {
  _secsyn_fast_queue($remote_server, 'SecureSyndicationEntityProfile', $server, TRUE);
  $children = server_management_server_load_by_parent($server->sid);
  foreach ($children as $child) {
    _server_management_queue_all_child_servers($remote, $child);
  }
}