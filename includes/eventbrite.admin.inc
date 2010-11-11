<?php
// $Id$

/**
 * @file
 * Manages and verifies the authentication params and settings
 *   used for communication with Eventbrite server.
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
  
  $form['eventbrite_cache_minimum'] = array(
    '#type' => 'textfield',
    '#title' => 'Minimum Cache Lifetime',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_cache_minimum', 5),
    // TODO: Provide better descriptions for min and max time
    '#description' => t('The minimum amount of time, in minutes, to cache results from the Eventbrite server.'),
  );
  
  $form['eventbrite_cache_maximum'] = array(
    '#type' => 'textfield',
    '#title' => 'Maximum Cache Lifetime',
    '#required' => TRUE,
    '#default_value' => variable_get('eventbrite_cache_maximum', 60),
    // TODO: Provide better descriptions for min and max time
    '#description' => t('The maximum amount of time, in minutes, to cache results from the Eventbrite server.'),
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
    // These form elements are set as #required, so if either are empty,
    //   there will be a form error set (just return FALSE)
    return FALSE;
  }

  if (_eventbrite_admin_verify_key_authentication($app_key, $user_key)) {
    return TRUE;
  }

  // Keys did not validate
  //   It's ok to display error messages, this will just be to the admin
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
  // The validate hook checked the keys against the eventbrite server
  //   by getting here we know that authentication was made
  // Meaning, we can set the valid keys variable to TRUE
  // TODO: Formalize validated status of Eventbrite keys
  variable_set('eventbrite_valid_authentication', TRUE);
  return TRUE;
}

/*
 * Function to test the keys entered by the user (will not happen often)
 * TODO: look into refactoring, this only gets called in seetings form
 *
 * This will not be hooked into the main request processing, which by default
 *   authenticates with the api key variable (ie rather than just entering one).
 */
function _eventbrite_admin_verify_key_authentication($app_key, $user_key) {
  // Return false if any keys are empty
  if (empty($app_key) || empty($user_key)) {
    return FALSE;
  }

  // This function builds its own request rather than tying into the module's
  //   request handling functions
  $query_string = 'app_key='. $app_key .'&user_key='. $user_key;

  // Send HTTP request to the server
  if ($result = drupal_http_request(EVENTBRITE_SERVER .'user_get?'. $query_string)) {
    // Process the result if there was one, and return simplexml_load_string($result->data)
    if (_eventbrite_common_process_result($result)) {
      // Successful API call
      $xml_object = $result->object;

      // TODO: write a populate user function much like populate event
      $eventbrite_user = new stdClass();
      foreach ($xml_object as $key => $value) {
        $eventbrite_user->$key = (string)$value;
      }
      // Return user, at this point no code is doing anything with it
      return $eventbrite_user;
    }
  }
}