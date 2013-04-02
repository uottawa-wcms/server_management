<?php
  if (!empty($server->label)) {
    print '<h1>' . $server->label . '</h1>';
  }
  print '<p class="parent_server">';
  if (!empty($server->parent_sid)) {
    $parent_server = server_management_load($server->parent_sid);
    print l($parent_server->label, 'admin/data/servers/' . $parent_server->sid);
  } else {
    print '<em>' . t('Top-Level') . '</em>';
  }
  print '</p>';

  $children = server_management_server_load_by_parent($server->sid);
  if (!empty($children)) {
    print '<div class="server-subsite-list">';
    print '<h2>' . t('Sub-Sites') . '</h2>';
    print '<ul>';
    foreach ($children as $child) {
      print '<li>' . l($child->label, 'admin/data/servers/' . $child->sid);
    }
    print '</ul>';
    print '</div>';
  }

  print render($content);

  print '<h2>' . t('Managed Configuration') . '</h2>';
  print l('+ Add Configuration', 'admin/data/servers/' . $server->sid . '/config/add');

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
  print theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('There are no configuration items for this server'),
  ));
