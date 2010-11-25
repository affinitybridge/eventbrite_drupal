<?php
// $Id$

function eventbrite_ui_event_form(&$form_state, $event_id = 0) {
  /* include form elements implemented by Date API */
  module_load_include('inc', 'date_api', 'date_api_elements.inc');
	
  if ($event_id) {
  	$event = eventbrite_get('event_get', $event_id);
  	
  	$form['event_id'] = array(
  	  '#type' => 'value',
  	  '#value' => $event->id,
  	);
  }
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => (isset($event->title)) ? $event->title : '',
  );
  
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => (isset($event->title)) ? $event->description : '',
  );
  
  // TODO: Set a variable for admin to configure default time
  //       For now just put it two weeks in advance
  $default_time = time() + (60 * 60 * 24 * 14);
  $default_format = 'Y-m-d ha';
  
  // TODO: allow admin to set format string
  //       For now just use basic
  $event_format = 'Y-m-d h:ia';

  $display_format = 'F-d-Y h:iA';

  $form['start_date'] = array(
    '#type' => 'date_select',
    '#title' => t('Start date'),
    '#default_value' => (isset($event->start_date)) ? format_date($event->start_date, 'custom', $event_format) : format_date($default_time, 'custom', $default_format),
    '#date_format' => $display_format,
    '#date_increment' => 5,
    '#date_label_position' => 'none',
    '#date_year_range' => '0:+1',  
  );
  $default_time += (60 * 60 * 3);

  $form['end_date'] = array(
    '#type' => 'date_select',
    '#title' => t('End date'),
    '#default_value' => (isset($event->end_date)) ? format_date($event->end_date, 'custom', $event_format) : format_date($default_time, 'custom', $default_format),
    '#date_format' => $display_format,
    '#date_increment' => 5,
    '#date_label_position' => 'none',
    '#date_year_range' => '0:+1',  
  );

  // TODO: Use site default timezone or user timezone, We need a proper timezone strategy
  if (isset($event->timezone)) {
  	$default_timezone = $event->timezone;
  }
  else {
    $default_timezone = variable_get('date_default_timezone', 0) / 3600;
  	// TODO: This is a terrible way to calculate timezone
  	if ($default_timezone > 0) {
  	  if ($default_timezone < 10) {
        $default_timezone = 'GMT+0'.$default_timezone;
   	  }
  	  else {
        $default_timezone = 'GMT+'.$default_timezone;
      }
  	}
  	elseif ($default_timezone < 0) {
  	  if ($default_timezone > -10) {
        $default_timezone = 'GMT-0'.$default_timezone;
   	  }
  	  else {
        $default_timezone = 'GMT-'.$default_timezone;
      }
  	}
  	else {
  	  $default_timezone = 'GMT';
  	}
  }
$default_timezone = 'GMT-7';
  $form['timezone'] = array(
    '#type' => 'value',
    '#value' => $default_timezone,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  
  return $form;
}

function eventbrite_ui_event_form_submit($form, &$form_state) {
  $event = new stdClass();
  
  if ($form_state['values']['event_id']) {
  	$event->event_id = $form_state['values']['event_id'];
  	drupal_set_message('you said it - '.$event->event_id);
  }
  $event->title = $form_state['values']['title'];
  $event->description = $form_state['values']['description'];
  
  $event->start_date = $form_state['values']['start_date'];
  $event->end_date = $form_state['values']['end_date'];

  $event->timezone = $form_state['values']['timezone'];
  
  if (isset($event->event_id)) {
  	// Event ID is present, this is an update
  	// TODO: Testing on event_id being returned doesn't make much sense, write better validation
    if ($event_id = eventbrite_save('event_update', $event)) {
      drupal_set_message(t('This event was successfully updated'));
      // TODO: Clear the events list cache so that the new event shows up
    }
    else {
      // Keys did not validate
      if ($errors = eventbrite_errors()) {
        foreach ($errors as $error_type => $error_msg) {
          drupal_set_message($error_type .': '. $error_msg, 'error');
        }
      }
      drupal_set_message('Could not update the event', 'error');
    }
  }
  else {
  	// Event ID is not present, this is an insert
    if ($event_id = eventbrite_save('event_new', $event)) {
      drupal_set_message(t('The new event was successfully created'));
      // TODO: Clear the events list cache so that the new event shows up
    }
    else {
      drupal_set_message('Could not update the event', 'error');
    }
  }
}


function eventbrite_ui_event_list() {
  // Pull all events from Eventbrite
  if ($events = eventbrite_get('user_list_events')) {
  	// TODO: check user access permissions before showing edit link (it will not work without perms, but dead link shouldn't show
    $header = array('Title', 'Description', 'Start Date', 'End Date', 'Edit Link');
    $rows = array();
    foreach ($events as $event) {
      $rows[] = array(
        l($event->title, 'eventbrite_ui/event/'.$event->id), 
        $event->description, 
        format_date($event->start_date, 'medium'), 
        format_date($event->end_date, 'medium'),
        l('edit', 'eventbrite_ui/event/'. $event->id .'/edit'),
      );
    }
    $output .= theme('table', $header, $rows);
  }
  return $output;
}

function eventbrite_ui_event($eventbrite_id) {
  // Pull all events from Eventbrite
  if ($event = eventbrite_get('event_get', $eventbrite_id)) {
  	$output = '<div class="eventbrite_ui-event">';
  	$output .= '<h4>'. l($event->title, 'eventbrite_ui/event/'.$event->id) .'</h4>';
  	$output .= '<p>'.$event->description.'</p>';
  	$output .= '<p>Date:'. format_date($event->start_date, 'medium') .' to '. format_date($event->end_date, 'medium') .'</p>';
  	$output .= '</div>';
  }
  else {
  	$output = t('This event could not be found');
  }
  return $output;
}


function eventbrite_ui_venue_list() {
  // Pull all events from Eventbrite
  if ($events = eventbrite_get('user_list_events')) {
  	// TODO: check user access permissions before showing edit link (it will not work without perms, but dead link shouldn't show
    $header = array('Title', 'Description', 'Start Date', 'End Date', 'Edit Link');
    $rows = array();
    foreach ($events as $event) {
      $rows[] = array(
        l($event->title, 'eventbrite_ui/event/'.$event->id), 
        $event->description, 
        format_date($event->start_date, 'medium'), 
        format_date($event->end_date, 'medium'),
        l('edit', 'eventbrite_ui/event/'. $event->id .'/edit'),
      );
    }
    $output .= theme('table', $header, $rows);
  }
  return $output;
}

function eventbrite_ui_default_payment_settings() {
//  $payment_settings_form = eventbrite_ui_payment_settings_form();
//  return system_settings_form($payment_settings_form);
  module_load_include('inc', 'eventbrite_ui', 'eventbrite_ui.forms');
  $form = eventbrite_ui_payment_settings_form();
  
  return $form;
}
