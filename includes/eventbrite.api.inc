<?php
// $Id$

/**
 * @file
 * Provides a function for each action in the Eventbrite API.
 */

  module_load_include('inc', 'eventbrite', 'includes/eventbrite.common');

  /*
 * This function fetches the user's list of events from Eventbrite
 * This is the call from version 1.x of the module
 */
function _eventbrite_api_get_events() {
  // Build a new request object
  $request = _eventbrite_common_new_request('user_list_events');

  // This function can now add any query string params to request

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

function _eventbrite_api_populate_event($event_xml) {
  $event = new stdClass();
  $event->id = (integer)$event_xml->id;
  $event->title = (string)$event_xml->title;
  $event->description = (string)$event_xml->description;

  return $event;
}