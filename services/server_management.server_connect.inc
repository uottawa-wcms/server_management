<?php

function server_management_service_invoke_server_connect($remote_server, $machine_name, $access_key, $delay) {
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
  $args = (array) $args;
  $server = server_management_load_by_name($args['machine_name']);
  if ($server->access_key == $args['access_key']) {
    watchdog('test', $message->getMessageVar('origin'));
    return TRUE;
  }
  return FALSE;
}