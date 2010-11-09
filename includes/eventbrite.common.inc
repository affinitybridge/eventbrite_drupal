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

function _eventbrite_common_process_result(&$result) {
  if ($result->code) {
    // Parse result for validity
    switch ($result->code) {
      case 200:
        $result->object = $xml_object = simplexml_load_string($result->data);

        if (isset($xml_object->error_message)) {
      	  $result->success = FALSE;
          // Error type is set, log to watchdog
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

        eventbrite_errors(t('The Eventbrite server can not be reached at this time.  Please contact the system administrator'));

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
  
  $request->id = isset($request->params['id']) ? $request->params['id'] : 0;
  // TODO: build query string for request
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
  
  // Check cache, if there is something there and it is recent enough then it's good,
  //   otherwise we'll actually have to make the call to the sever
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
        drupal_set_message('found a cached result, and it is still valid!');
        return $result;
      }
      else {
        drupal_set_message('found a cached result, unfortunately it is not still valid!');
      }
    }
  }
  // Send HTTP request to the server - nothing in the cache, so there's no other choice now  
  if ($result = drupal_http_request(EVENTBRITE_SERVER . $request->op .'?'. $request->query_string) ) {
    // Process the result if there was one, and return simplexml_load_string($result->data)
    $result->source = 'server';
    if (_eventbrite_common_process_result($result)) {
      if ($request->op_type == 'get' && $request->allowed_cache) {
      	if (isset($cached_result)) {
      	  // There was already a cached result, so we'll want to update either way
      	  if (strcmp($cached_result->result, $result->data)) {
      	  	$result->updated = TRUE;
      	  }
      	  else {
      	  	// XML date returned is the same, just update the timestamp
      	  	$result->updated = FALSE;
      	  }
      	  // Same update call whether result from server has changed or not
          _eventbrite_common_cache_result($result->data, $request, 'update');
      	}
      	else {
      	  // There was no cached result--so this should be thought of as an update either way
      	  //   (even if it was created earlier)
      	  //   (anyway at this point we've already made the DB call
      	  $result->updated = TRUE;
          _eventbrite_common_cache_result($result->data, $request, 'insert');
      	}
      }

      return $result;
    }
  }
}

function _eventbrite_common_check_cache($request) {
  $result = db_query(
    'SELECT * FROM {eventbrite_cache} WHERE op = "%s" and id = %d',
    $request->op,
    $request->id
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
      drupal_write_record('eventbrite_cache', $request, array('op', 'id'));
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
