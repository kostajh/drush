<?php

/**
 * Launch a Drush server over HTTP(s).
 */
function api_server_launch() {
  $php = drush_get_option('php');
  if ($headers = array_filter(drush_get_option_list('headers'))) {
    drush_cache_set('headers', $headers, 'drush-api');
  }
  if ($allowable_ips = array_filter(drush_get_option_list('allowable-ips'))) {
    drush_cache_set('allowable-ips', $allowable_ips, 'drush-api');
  }
  if ($http_host = array_filter(drush_get_option_list('allowable-http-hosts'))) {
    drush_cache_set('allowable-http-hosts', $http_host, 'drush-api');
  }
  drush_cache_set('api-request-handler', drush_get_option('command-callback', 'default'), 'drush-api');
  $prepend = DRUSH_BASE_PATH . '/commands/api/includes/api-prepend.php';
  $host = drush_get_option('host', 'localhost');
  $port = drush_get_option('port', '8888');
  $cmd = sprintf('%s -S %s:%s %s another-arg one two', $php, $host, $port, $prepend);
  return drush_shell_exec_interactive($cmd);
}