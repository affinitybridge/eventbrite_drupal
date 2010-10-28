<?php
// $Id$

/**
 * @file
 * Implements the request handling for communicating twith the Eventbrite server
 */

define('EVENTBRITE_SERVER', 'https://www.eventbrite.com/xml/');

/*
 * Build a request object
 */
function _eventbrite_common_new_request($op) {
  $request = new stdClass();
  $request->op = $op;
  $request->max_cache = 60 * variable_get('eventbrite_max_cache', 5);
  $request->user_key = variable_get('eventbrite_user_key', '');
  $request->params = array();

  switch ($op) {
    case 'user_list_events':
    case 'event_get':
      $request->op_type = 'get';
      break;
    case 'event_new':
      $request->op_type = 'save';
      // Set max_cache to 0 if this is a save operation (caching doesn't apply)
      $request->max_cache = 0;
      break;
  }
  if ($request->user_key) {
    return $request;
  }
}

function _eventbrite_common_process_result($result) {
  if ($result->code) {
    // Parse result for validity
    switch ($result->code) {
      case 200:
        $xml_data = simplexml_load_string($result->data);

        if (isset($xml_data->error_message)) {
          // Error type is set, log to watchdog
          // TODO: $request->op should be added to watchdog message
          // Also check for other error keys
          $watchdog_message = t('Error type: !error_type', array('!error_type' => $xml_data->error_type)) ."\n";
          $watchdog_message .= t('Error msg: !error_message', array('!error_message' => $xml_data->error_message)) ."\n";
          $watchdog_message .= t('Request: !request', array('!request' => $request)) ."\n";
          watchdog('eventbrite', $watchdog_message);

          _eventbrite_common_errors(t((string)$xml_data->error_message), (string)$xml_data->error_type);
          return;
        }

        // Successful API call, return the XML string rather than the simplexml object
        return $result->data;

      default:
        // Bad web connection
        $watchdog_message = t('Error code: !code', array('!code' => $result->code)) ."\n";
        $watchdog_message .= t('Error msg: !error', array('!error' => $result->error)) ."\n";
        $watchdog_message .= t('Request: !request', array('!request' => $request)) ."\n";
        watchdog('eventbrite', $watchdog_message);

        _eventbrite_common_errors(t('YOYO The Eventbrite server can not be reached at this time.  Please contact the system administrator'));

        return;
    }
  }
}

function _eventbrite_common_process_request($request) {
  if (!$app_key = variable_get('eventbrite_app_key', '')) {
    // TODO: Fire a better error for having no appication key
    drupal_set_message('No Application key found', 'error');
    return FALSE;
  }

  // TODO: expose a hook here to let other modules change the request
  
  // Set cache to 0 if op_type is save--submodules should not be changing this
  if ($request->op_type == 'save') {
    $request->max_cache = 0;
  }
  
  // TODO: build query string
  $param_list = $request->params;
  $request->params['user_key'] = $request->user_key;
  $request->params['app_key'] = $app_key;
  $param_list = array();
  foreach ($request->params as $key => $value) {
    $param_list[] = $key .'='. urlencode($value);
  }
  $request->query_string = join('&', $param_list);

  // TODO: check request against cache in the DB, return if a suitable row is found
  // only check if max_cache is non zero
  if ($request->max_cache) {
    if ($cached_result = _eventbrite_common_check_cache($request)) {
      // TODO: Check if cached result is recent enough to use
      // If so, return it
      $now = time();
      if (($cached_result->timestamp + $request->max_cache) > $now) {
        // Cache is still valid
        drupal_set_message('found a cached result, and it is still valid!');
        return simplexml_load_string($cached_result->result);
      }
      else {
        drupal_set_message('found a cached result, unfortunately it is not still valid!');
      }
    }
  }
  // Send HTTP request to the server - nothing in the cache, so there's no other choice now  
  if ($result = drupal_http_request(EVENTBRITE_SERVER . $request->op .'?'. $request->query_string) ) {
    // Process the result if there was one, and return simplexml_load_string($result->data)
    if ($server_result = _eventbrite_common_process_result($result)) {
      // Only cache cachable queries
      if ($request->max_cache) {
        if ($cached_result) {
          _eventbrite_common_cache_result($server_result, $request, 'update');
        }
        else {
        	_eventbrite_common_cache_result($server_result, $request, 'insert');
        }
      }
      return simplexml_load_string($server_result);
    }
  }

}

function _eventbrite_common_check_cache($request) {
  $result = db_query(
    'SELECT * FROM {eventbrite_cache} WHERE op = "%s" and query_string = "%s"',
    $request->op,
    $request->query_string
  );

  if ($result) {
    if ($row = db_fetch_object($result)) {
      return $row;
    }
  }
}

function _eventbrite_common_cache_result($result, $request, $db_op) {
  $request->result = $result;
  $request->timestamp = time();
  switch ($db_op) {
    case 'insert':
      drupal_write_record('eventbrite_cache', $request);
      break;
    case 'update':
      drupal_write_record('eventbrite_cache', $request, array('op', 'query_string'));
      break;
  }
}