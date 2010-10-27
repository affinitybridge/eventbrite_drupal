<?php
// $Id$

/**
 * @file
 * Provides a function for each action in the Eventbrite API.
 */

  module_load_include('inc', 'eventbrite', 'includes/eventbrite.common');

/*
 * This function fetches the user's list of events from Eventbrite
 */
function _eventbrite_api_get_events() {
  // Build a new request object
  $request = _eventbrite_common_new_request('user_list_events');

  // It's too bad that this returns XML data by default, I think it needs to be though
  if ($xml_data = _eventbrite_common_process_request($request)) {
    $events = array();
    foreach ($xml_data as $key => $xml_event) {
      if ($key == 'event' && $event = _eventbrite_api_populate_event($xml_event)) {
        $events[$event->id] = $event;
      }
    }
    return $events;
  }
}

/*
 * This function fetches one events by Id
 */
function _eventbrite_api_get_event($event_id) {
  // Build a new request object
  $request = _eventbrite_common_new_request('event_get');

  $request->params['id'] = $event_id;

  if ($xml_data = _eventbrite_common_process_request($request)) {
  	$event = _eventbrite_api_populate_event($xml_data);
    return $event;
  }
}

function _eventbrite_api_populate_event($event_data) {
  $event = new stdClass();
  $event->id = (integer)$event_data->id;
  $event->title = (string)$event_data->title;
  $event->description = (string)$event_data->description;
  $event->start_date = strtotime((string)$event_data->start_date);
  $event->end_date = strtotime((string)$event_data->end_date);
  
  return $event;
}

function _eventbrite_api_event_new($event) {
  $request = _eventbrite_common_new_request('event_new');

  // Add event data to request
  $request->params['title'] = $event->title;
  $request->params['description'] = $event->description;
  $request->params['start_date'] = $event->start_date;
  $request->params['end_date'] = $event->end_date;
  $request->params['timezone'] = $event->timezone;

  // XML data will only come back if no error
  if ($xml_data = _eventbrite_common_process_request($request)) {
    $message = (string)$xml_data->message;
    $event_id = (integer)$xml_data->id;
    watchdog('eventbrite', "Message: %message\nEventbrite Event ID: %id", array('%message' => $message, '%id' => $id));
    return $event_id;
  }
}

function _eventbrite_api_event_update($event) {
  $request = _eventbrite_common_new_request('event_new');

  // Add event data to request
  $request->params['id'] = $event->id;
  $request->params['title'] = $event->title;
  $request->params['description'] = $event->description;
  $request->params['start_date'] = $event->start_date;
  $request->params['end_date'] = $event->end_date;
  $request->params['timezone'] = $event->timezone;

  // XML data will only come back if no error
  if ($xml_data = _eventbrite_common_process_request($request)) {
    $message = (string)$xml_data->message;
    $event_id = (integer)$xml_data->id;
    watchdog('eventbrite', "Message: %message\nEventbrite Event ID: %id", array('%message' => $message, '%id' => $id));
    return $event_id;
  }
}
