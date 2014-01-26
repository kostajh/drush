<?php

use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;


/**
 * Class DrushWebSocket.
 * @property string response
 */
class DrushWebSocket implements MessageComponentInterface {

  protected $clients;
  protected $response;
  protected $from;
  protected $request;
  protected $alias;
  protected $command;
  protected $args;
  protected $options;

  /**
   * Constructor.
   */
  public function __construct() {
    $this->clients = new \SplObjectStorage();
  }

  /**
   * Actions to take when new connection is opened.
   */
  public function onOpen(ConnectionInterface $conn) {
    // Store the new connection.
    $this->clients->attach($conn);
    drush_log(dt("New connection #!resource received from !ip",
      array('!resource' => $conn->resourceId, '!ip' => $conn->remoteAddress)), 'ok');
  }

  /**
   * Action when message is received.
   */
  public function onMessage(ConnectionInterface $from, $request) {

    foreach ($this->clients as $client) {
      if ($from == $client) {
        $this->request = $request;
        $this->from = $from;
        $this->response = '';
        drush_log(dt('Request from #!resource at IP !ip: !request',
          array(
            '!resource' => $client->resourceId,
            '!ip' => $client->remoteAddress,
            '!request' => trim($request))),
          'ok');
        $this->processRequest();
        drush_log(dt('Processed request in !seconds.'), 'ok');
        $client->send($this->response);
      }
    }
  }

  /**
   * Process the incoming request.
   */
  protected function processRequest() {
    // Check if $from is allowed.
    if (!$this->validateFrom()) {
      return FALSE;
    }
    elseif (!$this->validateRequest()) {
      return FALSE;
    }
    else {
      $this->runCommand();
    }
  }

  /**
   * Check that the requester is coming from an allowed host and/or IP.
   */
  protected function validateFrom() {
    return TRUE;
  }

  /**
   * Validate the request.
   *
   * Check if the alias is valid; if the command exists; and if the options are
   * valid.
   */
  protected function validateRequest() {
    $parsed = parse_url($this->request);
    $args = explode('/', $parsed['path']);
    $this->alias = array_shift($args);
    $this->command = array_shift($args);
    $this->args = $args;
    $this->options = explode('&', $parsed['query']);
    return TRUE;
  }

  /**
   * Run the requested command through drush_invoke_process().
   */
  protected function runCommand() {
    $ret = drush_invoke_process($this->alias, $this->command, $this->args, $this->options, FALSE);
    $this->response = json_encode($ret);
  }

  /**
   * Action when connection is closed.
   */
  public function onClose(ConnectionInterface $conn) {
    // The connection is closed.
    $this->clients->detach($conn);
    drush_log(dt('Closing connection #!resource from IP !ip',
      array('!resource' => $conn->resourceId, '!ip' => $conn->remoteAddress)), 'ok');
  }

  /**
   * Log errors.
   */
  public function onError(ConnectionInterface $conn, \Exception $e) {
    drush_set_error('DRUSH_WEB_SOCKET_ERROR', dt('An error occurred: !msg',
       array('!msg' => $e->getMessage())));
    $conn->close();
  }
}