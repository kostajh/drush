<?php

/**
 * For the `custom` request handler, call the custom command callback.
 *
 * It is up to the implementer to return a valid response. At this point
 * IP and Host have been checked, but the implementer may want to reference
 * these before deciding if a command is allowed.
 */
function api_request_handler_process_request($ip, $host, $request) {
  $command_callback = drush_get_option('command-callback');
  return drush_invoke_process('@none', $command_callback, $ip, $host, $request);
}

