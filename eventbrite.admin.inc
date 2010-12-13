<?php
// $Id$

/**
 * @file
 * Manages and verifies the authentication params and settings
 *   used for communication with Eventbrite server.
 */

module_load_include('inc', 'eventbrite', 'eventbrite.common');

/*
 * Admin settings form for the module
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

  // TODO: Think more about how min and max cache are being used with the Eventbrite API
  // TODO: Provide better descriptions for min and max cache based on implementation
  $form['eventbrite_cache_minimum'] = array(
    '#type' => 'textfield',
    '#title' => 'Minimum Cache Lifetime',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_cache_minimum', 5),
    '#description' => t('The minimum amount of time, in minutes, to cache results from the Eventbrite server.'),
  );

  $form['eventbrite_cache_maximum'] = array(
    '#type' => 'textfield',
    '#title' => 'Maximum Cache Lifetime',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_cache_maximum', 60),
    '#description' => t('The maximum amount of time, in minutes, to cache results from the Eventbrite server.'),
  );

  // Set the Authentication flag to valid on form submit
  $form['eventbrite_is_valid_auth'] = array(
    '#type' => 'value',
    '#value' => TRUE,
  );

  return system_settings_form($form);
}

/*
 * Validate admin settings form
 */
function eventbrite_admin_settings_validate($form, &$form_state) {
  $eventbrite_app_key = $form_state['values']['eventbrite_app_key'];
  $eventbrite_user_key = $form_state['values']['eventbrite_user_key'];

  // Application Key and User Key are both required for the form to validate
  if (empty($eventbrite_app_key) || empty($eventbrite_user_key)) {
    // These form elements are set as #required, so exit without form_set_error()
    return FALSE;
  }

  // Validate the keys
  if (_eventbrite_admin_authenticate_keys($eventbrite_app_key, $eventbrite_user_key)) {
    // Keys were successfully authenticated by the Eventbrite server
    return TRUE;
  }

  // Keys did not validate, display error messages to the admin
  // TODO: Remove this error msg handling, as common_process_result will be removing calls to eventbrite_errors()
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
 * Function to test keys entered by the user
 *
 * This function builds its own requests, using the arguments as keys, rather than
 *   the module's variables.  This function DOES use the module's result processing
 *   T
 *   and takes in keys rather than using the module's variables to authenticate
 *   
 * Returns the user that was authenticated
 */
function _eventbrite_admin_authenticate_keys($app_key, $user_key) {
  // Return false if any keys are empty
  if (empty($app_key) || empty($user_key)) {
    return FALSE;
  }

  // Build query string for user_get request
  $query_string = 'app_key='. $app_key .'&user_key='. $user_key;

  // Send HTTP request to the server
  if ($result = drupal_http_request(EVENTBRITE_SERVER .'user_get?'. $query_string)) {
    // Process the result if there was one, and return simplexml_load_string($result->data)
    if (_eventbrite_common_process_result($result)) {
      // Successful API call
      $xml_object = $result->object;

      // TODO: write a populate user function much like _eventbrite_api_populate_event()
      $eventbrite_user = new stdClass();
      foreach ($xml_object as $key => $value) {
        $eventbrite_user->$key = (string)$value;
      }

      // Return authenticated user
      return $eventbrite_user;
    }
  }
}

