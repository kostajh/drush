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
      'drush-path' => 'The path to the Drush executable, e.g. /usr/local/bin/drush, to use when processing a request.',
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
    'engines' => array(
      'api_request_handler' => array(
        'add-options-to-command' => FALSE,
      ),
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
  // Store this setting in the cache so the `api-request` command
  // can load it later.
  $request_handler = drush_get_option('request-handler');
  drush_cache_set('request_handler', $request_handler, 'drush-api');
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
function drush_api_request($request, $http_host = NULL, $ip_address = NULL) {
  // Check if access is allowed.
  if (!_api_request_allow($request, $http_host, $ip_address)) {
    return _api_access_denied();
  }
  return api_request_handler_process_request($ip_address, $http_host, $request);
}

/**
 * Check if the request is allowed.
 */
function _api_request_allow($request, $http_host, $ip_address) {

  // TODO: Allow for validating $request.
  // Check if allowable IPs are defined.
  $allowable_ips = drush_get_option('allowable-ips', array());
  if ($allowable_ips) {
    if ($ip_address && !in_array($ip_address, $allowable_ips)) {
      return FALSE;
    }
  }

  // Check if HTTP_HOST matches.
  $allowable_http_hosts = drush_get_option('allowable-http-hosts', array());
  if ($allowable_http_hosts) {
    if (!in_array($http_host, $allowable_http_hosts)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Parse an incoming request into alias, command, args and options.
 */
function _api_parse_request($request) {
  $parsed = parse_url($request);
  $args = explode('/', $parsed['path']);
  $alias = array_shift($args);
  $command = array_shift($args);
  $options = explode('&', $parsed['query']);
  return array(
    'alias' => $alias,
    'command' => $command,
    'args' => $args,
    'options' => $options,
  );
}

/**
 * Run a command via drush_invoke_process() and return the output.
 */
function _api_run_command($alias, $command, $args = array(), $options = array()) {
  return drush_invoke_process($alias, $command, $args, $options, FALSE);
}

/**
 * An access denied array to return to drush_api_request().
 */
function _api_access_denied() {
  return array(
    'response_code' => 403,
    'error_status' => 1,
    'error_log' => dt('Access denied.'),
  );
}

/**
 * An error array to return to drush_api_request().
 */
function _api_error() {
  return array(
    'error_status' => 1,
    'error_log' => dt('Invalid request.'),
  );
}