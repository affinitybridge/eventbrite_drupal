<?php
// $Id$

/**
 * @file
 * Implements the request handling for communicating with the Eventbrite server
 */

define('EVENTBRITE_SERVER', 'https://www.eventbrite.com/xml/');

/*
 * Build a request object
 */
function _eventbrite_common_new_request($op, $op_type) {
  $request = new stdClass();
  $request->op = $op;
  $request->op_type = $op_type;
  $request->allowed_cache = ($op == 'get') ? 60 * variable_get('eventbrite_cache_minimum', 5) : 0;
  $request->user_key = variable_get('eventbrite_user_key', '');
  $request->params = array();

  if ($request->user_key) {
    return $request;
  }
}

/*
 * Process a request object that has been modified by an API function
 */
function _eventbrite_common_process_request($request) {
  // TODO: put a better validation system in place for eventbrite auth creds
  if (!$app_key = variable_get('eventbrite_app_key', '')) {
    // TODO: Fire a better error for having no appication key
    drupal_set_message('No Application key found', 'error');
    return FALSE;
  }

  // TODO: expose a hook here to let other modules change the request

  // Build query string from $request object
  $request->id = isset($request->params['id']) ? $request->params['id'] : 0;
  $request->params['user_key'] = $request->user_key;
  $request->params['app_key'] = $app_key;
  $param_list = array();
  foreach ($request->params as $key => $value) {
    $param_list[] = $key .'='. urlencode($value);
  }
  $request->query_string = join('&', $param_list);

  // Check cache, if there is something there and it is recent enough then use it
  //   otherwise we'll actually have to make the call to the server
  if ($request->op_type == 'get' && $request->allowed_cache) {
    if ($cached_result = _eventbrite_common_check_cache($request)) {
      // There is a cached result, check if it is recent enough to use
      $now = time();
      if (($cached_result->timestamp + $request->allowed_cache) > $now) {
        // Cache is still valid
        // Build fake result object out of the cached_result
        $result = new stdClass();
        $result->data = $cached_result->result;
        $result->object = simplexml_load_string($result->data);
        $result->source = 'cache';
        $result->updated = FALSE;
        drupal_set_message('Found a valid cached result.');
        return $result;
      }
      else {
        drupal_set_message('Found an expired cached result.');
      }
    }
  }

  // Send HTTP request to the server - since we don't have fresh data
  if ($result = drupal_http_request(EVENTBRITE_SERVER . $request->op .'?'. $request->query_string) ) {
    // Process the result if there was one, and return simplexml_load_string($result->data)
    if (_eventbrite_common_process_result($result)) {
      $result->source = 'server';

      // If this was a get operation then cache the results
      if ($request->op_type == 'get' && $request->allowed_cache) {
        if (isset($cached_result)) {
          // There was already a cached result, so we'll need to update the cache
          if (strcmp($cached_result->result, $result->data)) {
            // XML data returned has been updated, flag it so that other modules can refresh data
            $result->updated = TRUE;
          }
          else {
            // XML data returned is the same, just update the timestamp
            $result->updated = FALSE;
          }
          // Same update call whether result from server has changed or not
          _eventbrite_common_cache_result($result->data, $request, 'update');
        }
        else {
          // There was no cached result, so we'll need to insert into the cache
          // And flag the data as new for other modules to save it
          $result->updated = TRUE;
          _eventbrite_common_cache_result($result->data, $request, 'insert');
        }
      }

      return $result;
    }
  }
}

/*
 * Process the result returned by the server
 */
function _eventbrite_common_process_result(&$result) {
  // Check HTTP return code
  if ($result->code) {
    switch ($result->code) {
      // Valid HTTP Request
      case 200:
        $result->object = $xml_object = simplexml_load_string($result->data);

        // If error type is set, log to watchdog
        if (isset($xml_object->error_message)) {
          $result->success = FALSE;
          // TODO: $request->op should be added to watchdog message
          // Also check for other error keys
          $watchdog_message = t('Error type: !error_type', array('!error_type' => $xml_object->error_type)) ."\n";
          $watchdog_message .= t('Error msg: !error_message', array('!error_message' => $xml_object->error_message)) ."\n";
          $watchdog_message .= t('Request: !request', array('!request' => $result->request)) ."\n";
          watchdog('eventbrite', $watchdog_message);

          eventbrite_errors(t((string)$xml_object->error_message), (string)$xml_object->error_type);
          return;
        }

        $result->success = TRUE;
        return TRUE;

      default:
        $result->success = FALSE;
        // Bad web connection
        $watchdog_message = t('Error code: !code', array('!code' => $result->code)) ."\n";
        $watchdog_message .= t('Error msg: !error', array('!error' => $result->error)) ."\n";
        $watchdog_message .= t('Request: !request', array('!request' => $result->request)) ."\n";
        watchdog('eventbrite', $watchdog_message);

        // TODO: Pick the best error connection
        eventbrite_errors(t('The Eventbrite server can not be reached at this time.  Please contact the system administrator'));

        return;
    }
  }
}


function _eventbrite_common_check_cache($request) {
  $result = db_query(
    'SELECT * FROM {eventbrite_cache} WHERE op = "%s" AND id = %d AND query_string = "%s"',
    $request->op,
    $request->id,
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
      drupal_write_record('eventbrite_cache', $request, array('op', 'id', 'query_string'));
      break;
  }
}

function _eventbrite_common_cache_expire($op, $id = 0) {
  $values = array(
    'op' => $op,
    'id' => $id,
    'timestamp' => 0,
  );
  drupal_write_record('eventbrite_cache', $values, array('op', 'id'));
}
