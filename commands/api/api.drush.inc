<?php

/**
 * @file
 * Code for Drush Web Service API functionality.
 */

/**
 * Implements hook_drush_help().
 */
function api_drush_help($section) {
  switch ($section) {
    case 'drush:api':
      return dt('Expose Drush as a web service. Allow clients to interact with Drush over HTTP or WebSockets.');
  }
}

/**
 * Implements hook_drush_command().
 */
function api_drush_command() {
  $items['api-server'] = array(
    'description' => 'Expose Drush as a web service.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'engines' => array(
      'api_server' => array(
        'add-options-to-command' => FALSE,
      ),
      'api_request_handler' => array(
        'add-options-to-command' => FALSE,
      ),
    ),
    'options' => array(
      'host' => 'The hostname to use.',
      'port' => 'The port to use.',
      'allowable-ips' => 'A comma-separated list of IPs that may use the web service.',
      'allowable-http-hosts' => 'A comma-separated list of $_SERVER[\'HTTP_HOST\'] host names that will be accepted from the requester.',
    ),
  );
  $items['api-request'] = array(
    'description' => 'Return data from Drush to the caller.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'options' => array(),
    'arguments' => array(
      'drush' => 'The Drush executable to use.',
      'request' => 'The request to execute',
      'http-host' => 'The HTTP Host of the requester',
    ),
    'outputformat' => array(
      'default' => 'json',
      'engines' => 'json',
      'pipe-format' => 'json',
      'options' => array('json'),
    ),
  );
  return $items;
}

/**
 * Implements hook_drush_COMMAND_init().
 */
function drush_api_server_init() {
  // Clear our "settings" cache.
  drush_cache_clear_all(NULL, 'drush-api');
}

/**
 * Command callback for `drush api-server`.
 *
 * Launch a new Drush API server using a specified engine type.
 */
function drush_api_server() {
  api_server_launch();
}

/**
 * Implements hook_COMMAND_validate().
 */
function drush_api_server_validate() {
  if (drush_get_option('server-type') == 'http') {
    // Verify that user is on PHP 5.4+ for HTTP option.
    if (version_compare(phpversion(), '5.4') < 0) {
      return drush_set_error('DRUSH_API_SERVER_REQUIREMENTS_ERROR', dt('You need PHP 5.4 to use the HTTP option for the API Server command. You are running PHP !version', array('!version' => phpversion())));
    }
  }
  return TRUE;
}

/**
 * Implements hook_drush_engine_type_info().
 *
 * Inform Drush about our server type and request handler engines.
 */
function api_drush_engine_type_info() {
  return array(
    'api_server' => array(
      'description' => 'Determine how to provide access to Drush over the web.',
      'option' => 'server-type',
      'default' => 'websocket',
      'add-options-to-command' => FALSE,
    ),
    'api_request_handler' => array(
      'description' => 'Determine how to process API requests to Drush.',
      'option' => 'request-handler',
      'default' => 'default',
      'add-options-to-command' => FALSE,
    ),
  );
}

/**
 * @defgroup engines Engine types
 * @{
 */

/**
 * Implements hook_drush_engine_ENGINE_TYPE().
 *
 * Provides information about the api-server-type engine.
 */
function api_drush_engine_api_server() {
  return array(
    'http' => array(
      'description' => 'Serve Drush over HTTP using PHP\'s built-in server. This should not be used on a public network',
      'options' => array(
        'headers' => 'A comma-separated list of additional headers to set.',
      ),
    ),
    'websocket' => array(
      'description' => 'Serve Drush over a websocket. Use SSL if using over a public network.',
    ),
  );
}

/**
 * Implements hook_drush_engine_ENGINE_TYPE().
 *
 * Provide information about the api-request-handler engine type.
 */
function api_drush_engine_api_request_handler() {
  return array(
    'default' => array(
      'description' => 'Default API request handler. Responsible for parsing the incoming request and returning output.',
    ),
    'custom' => array(
      'description' => 'Use a custom API request handler. If you choose this option, you must pass the `--command-callback` option with a valid Drush command to parse requests and return output.',
      'options' => array(
        'command-callback' => array(
          'description' => 'A valid Drush command to use for parsing a request and returning data.',
          'required' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * @} End of "Engine types".
 */

/**
 * Command callback for `drush api-request`.
 */
function drush_api_request($drush, $request, $http_host = NULL) {

  $json_header = array('Content-Type: application/json');
  if (!_drush_api_request_allow($request, $http_host)) {
    $response['response_code'] = '403';
    $response['headers'] = $json_header;
    $response['output'] = json_encode('Access denied.');
    return $response;
  }
  $cmd = sprintf('%s %s --backend', $drush, $request);
  $out = shell_exec($cmd);
  $response = drush_backend_parse_output($out);
  $response_code = 200;
  // Check if we have output in JSON format.
  if (!json_decode($response['output']) && $response['error_status'] !== 1) {
    // If object is set and we did not get JSON in the output, return object.
    if (count($response['object'])) {
      $response['output'] = json_encode($response['object']);
    }
    else {
      // Assume we ran a command that doesn't provide output.
      $response['output'] = json_encode(sprintf('Successfully ran the command: "%s %s"', $drush, $request));
    }
  }
  // If it's an error, encode the response.
  if ($response['error_status'] == 1) {
    $response['output'] = json_encode($response['error_log']);
    $response_code = 500;
  }
  // Set headers. We always return JSON.
  if ($headers = drush_cache_get('headers', 'drush-api')) {
    $response['headers'] = array_merge($headers->data, $json_header);
  }
  else {
    $response['headers'] = $json_header;
  }
  // Set the response code.
  $response['response_code'] = $response_code;
  return $response;
}

/**
 * Check if the request is allowed.
 */
function _drush_api_request_allow($request, $http_host) {
  // Check if allowable IPs are defined.
  if ($allowable_ips = drush_cache_get('allowable-ips', 'drush-api')) {
    if (isset($_SERVER['REMOTE_ADDR']) && !in_array($_SERVER['REMOTE_ADDR'], $allowable_ips->data)) {
      return FALSE;
    }
  }
  // Check if HTTP_HOST matches.
  if ($allowable_http_hosts = drush_cache_get('allowable-http-hosts', 'drush-api')) {
    if (!in_array($http_host, $allowable_http_hosts->data)) {
      return FALSE;
    }
  }
  return TRUE;
}
