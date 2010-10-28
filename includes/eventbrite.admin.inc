<?php
// $Id$

/**
 * @file
 * Manages and verifies the authentication params and settings used for communication with Eventbrite server.
 */

module_load_include('inc', 'eventbrite', 'includes/eventbrite.common');

/*
 * Module settings form
 */
function eventbrite_admin_settings() {
  $form = array();

  $form['eventbrite_app_key'] = array(
    '#type' => 'textfield',
    '#title' => 'Application Key (API Key)',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_app_key', ''),
    '#description' => t('You must have an Eventbrite API key to use this module. You can obtain a key by <a href="http://www.eventbrite.com/api/key/" target="_blank">clicking here</a>.'),
  );
  $form['eventbrite_user_key'] = array(
    '#type' => 'textfield',
    '#title' => 'User Key',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_user_key', ''),
    '#description' => t('Your user key for secure authentication. You can find it by <a href="http://www.eventbrite.com/userkeyapi" target="_blank">clicking here</a>'),
  );
  
  $form['eventbrite_cache_lifetime'] = array(
    '#type' => 'textfield',
    '#title' => 'Default Cache Lifetime',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_cache_lifetime', 5),
    '#description' => t('The default amount of time, in minutes, to cache results from the Eventbrite server.  This may be overridden in hook_eventbrite_request_alter'),
  );
  
  return system_settings_form($form);
}

/*
 * Validate settings form
 */
function eventbrite_admin_settings_validate($form, &$form_state) {
  $app_key = $form_state['values']['eventbrite_app_key'];
  $user_key = $form_state['values']['eventbrite_user_key'];

  if (empty($app_key) || empty($user_key)) {
    // These form elements are set as #required, so if either are empty, there will be a form error set (just return FALSE)
    return FALSE;
  }

  if (_eventbrite_admin_verify_key_authentication($app_key, $user_key)) {
    return TRUE;
  }

  // Keys did not validate
  if ($errors = eventbrite_errors()) {
    foreach ($errors as $error_type => $error_msg) {
      switch ($error_type) {
        case 'Application Key Error':
          form_set_error('eventbrite_app_key', t($error_type) .': '. $error_msg);
          break;
        case 'Authentication Error':
          form_set_error('eventbrite_user_key', t($error_type) .': '. $error_msg);
          break;
        default:
          drupal_set_message($error_msg, 'error');
          break;
      }
    }
  }
  else {
    // Keys did not validate but no error was logged, this is the catch all
    drupal_set_message(t('The keys you entered could not be validated by the Eventbrite server.  Please check the values.'), 'error');
  }
  return FALSE;
}

/*
 * Process settings form
 */
function eventbrite_admin_settings_submit($form, &$form_state) {
  // The validate hook checked the keys against the eventbrite server...so if we got to submit then we should be all good!!!
  // Meaning, we can set the valid keys variable to TRUE
  // TODO: Formalize validated status of Eventbrite keys
  drupal_set_message('Submit hook successfully called');
  variable_set('eventbrite_valid_authentication', TRUE);
  return TRUE;
}

/*
 * Function to test the keys entered by the user
 * This may not need to happen very often
 * Will definitely need refactoring
 *
 * This will not be hooked into the main request processing, which by default
 *   authenticates with the api key variable (ie rather than just entering one).
 */
function _eventbrite_admin_verify_key_authentication($app_key, $user_key) {
  // Return false if any keys are empty
  if (empty($app_key) || empty($user_key)) {
    return FALSE;
  }

  // This function builds its own request rather than tying into the module's request handling functions
  $query_string = 'app_key='. $app_key .'&user_key='. $user_key;

  // Send HTTP request to the server - nothing in the cache, so there's no other choice now
  if ($result = drupal_http_request(EVENTBRITE_SERVER .'user_get?'. $query_string)) {
    // Process the result if there was one, and return simplexml_load_string($result->data)
    if ($result_string = _eventbrite_common_process_result($result)) {
      // Successful API call
      $xml_data = simplexml_load_string($result_string);

      $eventbrite_user = new stdClass();
      foreach ($xml_data as $key => $value) {
        $eventbrite_user->$key = (string)$value;
      }
      // TODO: possibly set some user_id values to integer (rather than string)
      //       (not sure if anything would ever be done with this returned user anyway)
      return $eventbrite_user;
    }
  }
}