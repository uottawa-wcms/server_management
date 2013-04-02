<?php

class ServerEntityController extends EntityAPIController {

  public function save($server, $transaction = NULL) {
    $tx = $transaction instanceof DatabaseTransaction ? $transaction : db_transaction();
    if (empty($server->status)) {
      $server->status = 0;
    }
    if (empty($server->access_key)) {
      $server->access_key = hash('sha256', uniqid($server->machine_name, TRUE));
    }
    if (empty($server->parent_sid)) {
      $server->parent_sid = 0;
    }
    parent::save($server, $tx);
  }

  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    $server = reset($entities);
    return theme('server', array(
      'server' => $server,
      'content' => $this->buildContent($server, $view_mode, $langcode),
      'view_mode' => $view_mode,
    ));
  }

}

?>
