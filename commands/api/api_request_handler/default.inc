<?php

/**
 * Process an incoming request via drush_invoke_process() and return results.
 */
function api_request_handler_process_request($ip, $host, $request) {
  $data = _api_parse_request($request);
  if (!$data) {
    return _api_error();
  }
  return _api_run_command($data['alias'], $data['command'], $data['args'], $data['options']);
}