<?php
// $Id$

/**
 * @file
 * Provides a function for each action in the Eventbrite API.
 */

module_load_include('inc', 'eventbrite', 'eventbrite.common');

/*
 * This function fetches the user's list of events from Eventbrite
 */
function _eventbrite_api_get_events() {
  // Build a new request object
  $request = _eventbrite_common_new_request('user_list_events', 'get');

  if ($result = _eventbrite_common_process_request($request)) {
    $xml_object = $result->object;
    $events = array();
    foreach ($xml_object as $key => $xml_event) {
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
  $request = _eventbrite_common_new_request('event_get', 'get');

  $request->params['id'] = $event_id;

  if ($result = _eventbrite_common_process_request($request)) {
    $xml_object = $result->object;
    $event = _eventbrite_api_populate_event($xml_object);
    return $event;
  }
}

function _eventbrite_api_populate_event($event_data) {
  $event = new stdClass();
  $event->id = (integer)$event_data->id;
  $event->title = (string)$event_data->title;
  $event->description = (string)$event_data->description;
  $event->start_date = (string)$event_data->start_date;
  $event->end_date = (string)$event_data->end_date;
  $event->timezone = $yar = (string)$event_data->timezone;

  return $event;
}

function _eventbrite_api_event_new($event) {
  $request = _eventbrite_common_new_request('event_new', 'save');

  // Add event data to request
  $request->params['title'] = $event->title;
  $request->params['description'] = $event->description;
  $request->params['status'] = $event->status;

  $request->params['start_date'] = $event->start_date;
  $request->params['end_date'] = $event->end_date;
  $request->params['timezone'] = $event->timezone;

  // Result will only return if no error
  if ($result = _eventbrite_common_process_request($request)) {
    // Populate the event coming back and return it
    $xml_object = $result->object;

    if ($event_id = (integer)$xml_object->id) {
      // Record in watchdog
      $message = (string)$xml_object->message;
      watchdog('eventbrite', "Message: %message\nEventbrite Event ID: %id", array('%message' => $message, '%id' => $event_id));

      // Expire the cache of event listing
      _eventbrite_common_cache_expire('user_list_events');
      return $event_id;
    }
  }
}

function _eventbrite_api_user_new($user) {
  $request = _eventbrite_common_new_request('user_new', 'save');

  // Add event data to request
  $request->params['email'] = $user->email;
  $request->params['passwd'] = $user->passwd;

  // Result will only return if no error
  if ($result = _eventbrite_common_process_request($request)) {
    // Populate the event coming back and return it
    $xml_object = $result->object;

    if ($user_id = (integer)$xml_object->id) {
      $eb_user = new stdClass();
      $eb_user->id = user_id;
      $eb_user->user_key = (string)$xml_object->user_key;
      
      // Record in watchdog
      $message = (string)$xml_object->message;
      watchdog(
        'eventbrite',
        "Message: %message\nEventbrite Subuser ID: %id\nUser key: %user_key",
        array('%message' => $message, '%id' => $eb_user->id, '%user_key' => $eb_uesr->user_key)
      );

      return $eb_user;
    }
  }
}

function _eventbrite_api_event_update($event) {
  $request = _eventbrite_common_new_request('event_update', 'save');

  // Add event data to request
  $request->params['event_id'] = $event->event_id;
  $request->params['title'] = $event->title;
  $request->params['description'] = $event->description;
  $request->params['status'] = $event->status;

  $request->params['start_date'] = $event->start_date;
  $request->params['end_date'] = $event->end_date;
  $request->params['timezone'] = $event->timezone;

  // XML data will only come back if no error
  if ($result = _eventbrite_common_process_request($request)) {
    // Populate the event coming back and return it
    $xml_object = $result->object;

    if ($event_id = (integer)$xml_object->id) {
      // Record in watchdog
      $message = (string)$xml_object->message;
      watchdog('eventbrite', "Message: %message\nEventbrite Event ID: %id", array('%message' => $message, '%id' => $event_id));

      // Expire the cache of event listing
      _eventbrite_common_cache_expire('event_get', $event_id);
      _eventbrite_common_cache_expire('user_list_events');

      return $event_id;
    }
  }
}

function _eventbrite_api_ticket_new($ticket) {
  $request = _eventbrite_common_new_request('ticket_new', 'save');

  // Add ticket data to request
  $request->params['event_id'] = $ticket->event_id;
  $request->params['name'] = $ticket->name;
  $request->params['quantity'] = $ticket->quantity;
  $request->params['price'] = $ticket->price;
  if (isset($request->params['description'])) {
    $request->params['description'] = $ticket->description;
  }
  if (isset($request->params['is_donation'])) {
    $request->params['is_donation'] = $ticket->is_donation;
  }

  // Result will only return if no error
  if ($result = _eventbrite_common_process_request($request)) {
    // Populate the event coming back and return it
    $xml_object = $result->object;

    // Record in watchdog
    $message = (string)$xml_object->message;
    $ticket_id = (integer)$xml_object->id;
    watchdog('eventbrite', "Message: %message\nEventbrite Ticket ID: %id", array('%message' => $message, '%id' => $ticket_id));

    return $ticket_id;
  }
}

function _eventbrite_api_ticket_update($ticket) {
  $request = _eventbrite_common_new_request('ticket_update', 'save');

  // Add ticket data to request
  $request->params['id'] = $ticket->id;
  $request->params['name'] = $ticket->name;
  $request->params['quantity'] = $ticket->quantity;
  $request->params['price'] = $ticket->price;
  if (isset($request->params['description'])) {
    $request->params['description'] = $ticket->description;
  }
  if (isset($request->params['is_donation'])) {
    $request->params['is_donation'] = $ticket->is_donation;
  }

  // XML data will only come back if no error
  if ($result = _eventbrite_common_process_request($request)) {
    // Populate the event coming back and return it
    $xml_object = $result->object;

    // Record in watchdog
    $message = (string)$xml_data->message;
    $ticket_id = (integer)$xml_object->id;
    watchdog('eventbrite', "Message: %message\nEventbrite Ticket ID: %id", array('%message' => $message, '%id' => $ticket_id));

    return $ticket_id;
  }
}
