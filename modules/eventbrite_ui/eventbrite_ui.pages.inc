<?php
// $Id$

function eventbrite_ui_event_form(&$form_state, $event_id = 0) {
  /*
  $form['id'] = array(
    '#type' => 'textfield',
    '#title' => 'ID',
    '#default_value' => $event_id,
  );
  */

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
  );
  
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
  );
  
  $form['start_date'] = array(
    '#type' => 'date',
    '#title' => t('Start date'),
  );
  
  $form['start_time'] = array(
    '#type' => 'textfield',
    '#title' => t('Start time'),
    '#description' => t('Example: 19:30'),
  );
  
  $form['end_date'] = array(
    '#type' => 'date',
    '#title' => t('End date'),
  );
  
  $form['end_time'] = array(
    '#type' => 'textfield',
    '#title' => t('End time'),
    '#description' => t('Example: 19:30'),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  
  return $form;
}

function eventbrite_ui_event_form_submit($form, &$form_state) {
  $event = new stdClass();
  
  $event->title = $form_state['values']['title'];
  $event->description = $form_state['values']['description'];
  
  $start_date = $form_state['values']['start_date'];
  $event->start_date = $start_date['year'] .'-'. $start_date['month'] .'-'. $start_date['day'];
  $event->start_date .= ' '. $form_state['values']['start_time'] .':00';

  $end_date = $form_state['values']['end_date'];
  $event->end_date = $end_date['year'] .'-'. $end_date['month'] .'-'. $end_date['day'];
  $event->end_date .= ' '. $form_state['values']['end_time'] .':00';

  $event->timezone = 'GMT+01';

  if ($event_id = eventbrite_save('event_new', $event)) {
    drupal_set_message(t('The new event was successfully created'));
    
    // TODO: Clear the events list cache so that the new event shows up
  }
  else {
  	drupal_set_message('Could not create the event', 'error');
  }
}


function eventbrite_ui_event_list() {
  // Pull all events from Eventbrite
  if ($events = eventbrite_get('user_list_events')) {
    $header = array('Title', 'Description', 'Start Date', 'End Date');
    $rows = array();
    foreach ($events as $event) {
      $rows[] = array(
        l($event->title, 'eventbrite_ui/event/'.$event->id), 
        $event->description, 
        format_date($event->start_date, 'medium'), 
        format_date($event->end_date, 'medium'),
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