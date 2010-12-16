<?php
// $Id$


function eventbrite_venue_form($default_values = array()) {
  $form = array();
  
  if (isset($default_values['venue_id'])) {  
    $form['venue_id'] = array(
      '#type' => 'value',
      '#value' => $default_values['venue_id'],
    );
  }
  
  // Name is the only required field
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => isset($default_values['name']) ? $default_values['name'] : '',
    '#required' => 1,
  );

  // TODO: Add Organizer.  Select box if there are more than one, otherwise set the default as a value
  
  $form['address'] = array(
    '#type' => 'textfield',
    '#title' => t('Address Line 1'),
    '#default_value' => isset($default_values['address']) ? $default_values['address'] : '',
  );

  $form['address2'] = array(
    '#type' => 'textfield',
    '#title' => t('Address Line 2'),
    '#default_value' => isset($default_values['address2']) ? $default_values['address2'] : '',
  );

  $form['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => isset($default_values['city']) ? $default_values['city'] : '',
  );

  $form['region'] = array(
    '#type' => 'textfield',
    '#title' => t('Region'),
    '#default_value' => isset($default_values['region']) ? $default_values['region'] : '',
  );

  $form['postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
    '#default_value' => isset($default_values['postal_code']) ? $default_values['postal_code'] : '',
  );

  $form['country'] = array(
    '#type' => 'textfield',
    '#title' => t('Country'),
    '#default_value' => isset($default_values['country']) ? $default_values['country'] : '',
  );

  $form['country_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Country Code'),
    '#default_value' => isset($default_values['country_code']) ? $default_values['country_code'] : '',
  );

  $form['longitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Longitude'),
    '#default_value' => isset($default_values['longitude']) ? $default_values['longitude'] : '',
  );

  $form['latitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Latitude'),
    '#default_value' => isset($default_values['latitude']) ? $default_values['latitude'] : '',
  );
  
  return $form;
}

// Temporary function
// TODO $form_errors wil have to turn into form_set_errors()
function eventbrite_venue_form_validate($values) {
  $form_errors = array();

  // TODO: Check that there isn't already a venue with this name for this user key
  /*
  if ($values['accept_paypal']) {
  	// If using PayPay check that paypal email is entered
    if (!valid_email_address($values['paypal_email'])) {
      // TODO: Does this need to actually verified that there is a PayPal account registered to this user
  	  $form_errors['paypal_email'] = t('Please provide a valid Paypal email.');
    }
  }
  */
  
  return $form_errors;
}

function eventbrite_venue_load($venue_id) {
  $result = db_query('SELECT * FROM {eventbrite_venue} WHERE venue_id = %d', $venue_id);
  if ($row = db_fetch_array($result)) {
  	return $row;
  }
}

/*
 * Venue Edit Form
 */
function eventbrite_venue($form_state, $venue_id) {
  $form = array();

  if ($venue = eventbrite_venue_load($venue_id)) {
    $form = eventbrite_venue_form($venue);
  
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update venue information'),
    );
  }
  return $form;
}

/*
 * Default Payment Settings Form Validate
 */
function eventbrite_venue_validate($form, &$form_state) {
  $form_values = $form_state['values'];

  eventbrite_venue_form_validate($form_values);
  return;
  
  $form_errors = eventbrite_venue_form_validate($form_values);
  foreach ($form_errors as $form_element => $error_msg) {
  	form_set_error($form_element, $error_msg);
  }
}

/*
 * Default Payment Settings Form Validate
 */
function eventbrite_venue_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  eventbrite_venue_save($form_values);
drupal_set_message('hooray');
return;
  $form_errors = eventbrite_form_validate($form_values);
  foreach ($form_errors as $form_element => $error_msg) {
  	form_set_error($form_element, $error_msg);
  }
}

function eventbrite_venue_options($required = FALSE) {
  $result = db_query('SELECT * FROM {eventbrite_venue}');
  $venue_options = array();
  if (!$required) {
    $venue_options[''] = '--';
  }
  while ($row = db_fetch_object($result)) {
  	$venue_options[$row->venue_id] = $row->name;
  }
  return $venue_options;
}

function eventbrite_venue_list() {
  // TODO: Get venues from Eventbrite server
  $venues = eventbrite_get('user_list_venues');

  if ($venues) {
    $header = array('Venue Name', 'Address' /* TODO: Add venue editing, 'Edit Link' */);
    $rows = array();
    foreach ($venues as $venue) {
      $address = $venue->address;
      if (!empty($venue->address2)) {
      	$address .= ', '.$venue->address2;
      }
      if (!empty($venue->city)) {
      	$address .= ', '.$venue->city;
      }
      if (!empty($venue->region)) {
      	$address .= ', '.$venue->region;
      }
      if (!empty($venue->country)) {
      	$address .= ', '.$venue->country;
      }
      if (!empty($venue->postal_code)) {
      	$address .= ', '.$venue->postal_code;
      }
      $rows[] = array(
//        l($venue->name, '/admin/settings/eventbrite/view-venue/'. $venue->venue_id), 
        $venue->name,
        $address,
        // TODO: Add venue editing back in
//        l('edit', 'admin/settings/eventbrite/list-venues/'. $venue->venue_id),
      );
    }
    return theme('table', $header, $rows);
  }
  
  return t('There are currently no venues conifigured for this user account.');
}

function eventbrite_venue_save($venue) {
  // TODO: Perhaps use the query with module in
  /*
  $result = db_query(
    'SELECT * FROM {eventbrite_venue} WHERE module = "%s" AND user_key = "%s" AND venue_id = %d',
    $venue->module,
    $venue->user_key,
    $venue->venue_id
  );
  */
//	print_r($venue);exit(4);
  $result = db_query(
    'SELECT * FROM {eventbrite_venue} WHERE user_key = "%s" AND venue_id = %d',
    $venue->venue_id
  );
  if ($row = db_fetch_object($result)) {
  	// This venue is already in the DB, just update
    drupal_write_record('eventbrite_venue', $venue, array('venue_id'));
  }
  else {
  	// This venue is not in the DB, must insert
    drupal_write_record('eventbrite_venue', $venue);
  }
}

