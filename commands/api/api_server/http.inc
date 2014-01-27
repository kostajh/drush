<?php

/**
 * Launch a Drush server over HTTP(s).
 */
function api_server_launch() {
  if (!drush_confirm('You should not use the HTTP server option on a public network. Are you sure you want to continue?')) {
    return FALSE;
  }
  $drush_path = drush_get_option('drush-path', NULL);
  if (!$drush_path) {
    return drush_set_error('DRUSH_PATH_NOT_DEFINED', dt('You must specify the path to the Drush executable with the `--drush-path` option.'));
  }
  $env_vars = array();
  $env_vars['DRUSH'] = $drush_path;

  $php = drush_get_option('php');
  if ($headers = array_filter(drush_get_option_list('headers'))) {
    $env_vars['DRUSH_API_HEADERS'] = $headers;
  }
  if ($allowable_ips = array_filter(drush_get_option_list('allowable-ips'))) {
    $env_vars['DRUSH_API_ALLOWABLE_IPS'] = $allowable_ips;
  }
  if ($allowable_http_hosts = array_filter(drush_get_option_list('allowable-http-hosts'))) {
    $env_vars['DRUSH_API_ALLOWABLE_HOSTS'] = $allowable_http_hosts;
  }
  if ($api_request_handler = drush_get_option('command-callback', 'http')) {
    $env_vars['DRUSH_API_REQUEST_HANDLER'] = ($api_request_handler == 'http') ? 'http' : $api_request_handler;
  }
  $prepend = DRUSH_BASE_PATH . '/commands/api/includes/api-prepend.php';
  $host = drush_get_option('host', 'localhost');
  $port = drush_get_option('port', '8888');
  $vars = '';
  foreach ($env_vars as $key => $value) {
    $vars .= $key . '=' . $value . ' ';
  }
  $cmd = sprintf('%s%s -d variables_order=EGPCS -S %s:%s %s -c %s',
    $vars,
    $php,
    $host,
    $port,
    $prepend,
    DRUSH_BASE_PATH . '/commands/api/includes/cli-server.ini');
  return drush_shell_exec_interactive($cmd);
}
