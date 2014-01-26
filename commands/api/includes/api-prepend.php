<?php

/**
 * @file
 * Code for exposing Drush as a web service using PHP's built in server.
 */

$response = _drush_api_request();
_drush_api_set_headers($response);
return _drush_api_set_output($response);

/**
 * Set the output and response code.
 */
function _drush_api_set_output($response) {
  // Print output.
  echo $response['output'];
  // Set response code.
  http_response_code($response['response_code']);
  return TRUE;
}
/**
 * Set the headers.
 */
function _drush_api_set_headers($response) {
  if (isset($response['headers']) && count($response['headers'])) {
    foreach ($response['headers'] as $header) {
      header($header);
    }
  }
}

/**
 * Make the request and get a response.
 */
function _drush_api_request() {
  // Take our request and pass to `drush web-service-request`.
  $request = urldecode(ltrim($_SERVER['REQUEST_URI'], '/'));
  if (!$request) {
    // Set a default command.
    $request = 'core-status';
  }
  $drush_executable = trim(shell_exec('which drush'));
  // We pass HTTP_HOST here because we don't have access to it in `api-request`.
  $command = sprintf('%s api-request %s %s %s', $drush_executable, escapeshellarg($drush_executable), escapeshellarg($request), escapeshellarg($_SERVER['HTTP_HOST']));
  // Log the command.
  error_log('Drush Web Service API: ' . $command);
  $response = json_decode(shell_exec($command), TRUE);
  return $response;
}
