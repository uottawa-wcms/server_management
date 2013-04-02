 <?php

class ServerConfigurationEntityController extends EntityAPIController {

  public function save($config, $transaction = NULL) {
    $tx = $transaction instanceof DatabaseTransaction ? $transaction : db_transaction();
    if (empty($config->inherit)) {
      $config->inherit = 0;
    }
    if (empty($config->uuid)) {
      $config->uuid = hash('sha256', uniqid($config->sid . $config->type, TRUE));
    }
    parent::save($config, $tx);
  }

}

?>
