<?php
// $Id$

function _eventbrite_manager_event_widget_process($element, $edit, &$form_state, $form) {
  // If #access is set and FALSE then don't show the widget
  // TODO: Verify whether or not we need this
  if (isset($element['#access']) && empty($element['#access'])) {
    return $element;
  }

  	
//	drupal_set_message('HERE IN WIDGE PROCESS');
	
  $ebid = isset($element['#default_value']['ebid']) ? $element['#default_value']['ebid'] : ''; 
  
  if ($ebid) {
  	// Field has been created before, don't show creator widget
  }
  else {
  	// Field is new, please show creator widget
  	$element['eventbrite_new'] = array(
  	  '#type' => 'radios',
  	  '#title' => t('Event source'),
  	  '#required' => $element['#required'],
  	  '#weight' => -20,
  	  '#options' => array(
  	    'load' => t('Load an event already created on the Eventbrite server'),
  	    'create' => t('Create a new event and save to Eventbrite server'),
  	  ),
  	  '#attributes' => array(
  	    'class' => 'eventbrite_cck-new-toggle',
  	  ),
  	);

  }

  
  
  //---------------- This chunk definitely needs to go into an api call
    $events = eventbrite_get('user_list_events');
  $options = array();
  $options[0] = '';
  foreach ($events as $event_id => $event) {
    $options[$event_id] = $event->title;
  }
  //---------------- This chunk definitely needs to go into an api call  ^^^^^^
  $element['load_ebid'] = array(
    '#title' => t( 'Eventbrite Event' ),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $defaults['ebid'],
    '#weight' => -5,
  	  '#attributes' => array(
  	    'class' => 'eventbrite_cck-new-load',
  	  ),
  );
  
  
  
  
  
  
  
  
  
  
  
  
  // TODO: Remove this $default variable
  // Make sure ebid gets set (it seems to require special handling)
//  drupal_set_message('hey value id - '.$element['#value']['ebid']);
  //drupal_set_message('hey default value id - '.$element['#default_value']['ebid']);
  
  // TODO: Figure out something better for default handling
  $defaults = $element['#value'];
  if (isset($element['#default_value']['ebid'])) {
  	$defaults['ebid'] = $element['#default_value']['ebid'];
  }
  $field_name = $element['#field_name'];
  
  $field = content_fields($field_name, $element['#type_name']);
  $delta = $element['#delta'];

  
//    drupal_set_message('Allow create/edit - '.$field['allow_create_edit']);
 // drupal_set_message('Allow loading - '.$field['allow_load']);
  
  $element['ebid'] = array(
    '#type' => 'value',
    '#value' => $defaults['ebid'],
  );
  
  $element['event_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Event Details'),
    '#weight' => -2,
    '#collapsible' => FALSE,
    '#attributes' => array(
      'class' => 'eventbrite_cck-new-create',
    ),
  );
  
  if (!empty($defaults['ebid'])) {
  	$edit_link = l(
  	  t('Edit this Event at Eventbrite'),
  	  'http://www.eventbrite.com/edit?eid='.$defaults['ebid'],
      array('attributes' => array('target' => '_blank'))
    );
    $element['event_details']['edit_link'] = array(
      '#type' => 'markup',
      '#value' => $edit_link,
      '#weight' => -10,
    );
  }
  
  $element['event_details']['title'] = array(
    '#title' => t('Event Title'),
    '#type' => 'textfield',
    '#default_value' => $defaults['title'],
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 0,
    '#required' => $element['#required'],
    '#parents' => array($field_name, $delta, 'title'),
  );

  // Don't show status to user on event creation, only on event edit, 
  //   status handling will be automatic on creation
//  if (!empty($defaults['ebid'])) {
    $element['event_details']['status'] = array(
      '#title' => t('Status'),
      '#type' => 'radios',
      '#default_value' => isset($defaults['status']) ? $defaults['status'] : 'draft',
      '#weight' => 1,
      '#required' => 1,
      '#options' => array('draft' => t('Draft'), 'live' => t('Live')),
      '#parents' => array($field_name, $delta, 'status'),
    );
    
//  }
  
  
  $element['event_details']['description'] = array(
    '#title' => t('Event Description'),
    '#type' => 'textarea',
    '#default_value' => $defaults['description'],
    '#weight' => 1,
    '#parents' => array($field_name, $delta, 'description'),
  );
  
  $element['when'] = _eventbrite_manager_event_widget_process_when($element);

  $element['where'] = _eventbrite_manager_event_widget_process_where($element);
  
  $element['tickets'] = _eventbrite_manager_event_widget_process_tickets($element);
  
  $element['payment'] = _eventbrite_manager_event_widget_process_payment($element);
  
  $element['discounts'] = _eventbrite_manager_event_widget_process_discounts($element);
  
  // TODO: perhaps only show this chunk of the widget if the event is already created
  /* Hide data & capacity for now
  $element['data'] = array(
    '#tree' => 'true',
    '#weight' => 4,
  //    '#access' => !empty($item['ebid']),
  );

  $element['data']['capacity'] = array(
    '#type' => 'textfield',
    '#title' => t('Capacity'),
    '#value' => isset($element['#value']['data']['capacity']) ? $element['#value']['data']['capacity'] : '',
    '#size' => 10,
    '#maxlength' => 6,
  );
  */
  
  // Put $field into $form_state for validate() function
  $form_state['#field_info'][$field['field_name']] = $field;

  return $element;
}

function _eventbrite_manager_event_widget_process_when($element) {
  $field_name = $element['#field_name'];
	  $delta = $element['#delta'];
//  $fieldset_name = 'when'; Not needed
  $field = content_fields($field_name, $element['#type_name']);
  
  // TODO: Add configurability for default date values
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('When'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array(
      'class' => 'eventbrite_cck-new-create',
    ),
  );
  
  // TODO: Add configurability for default date
  //       For now, set default event date 30 days in the future
  $default_ts = time() + (60 * 60 * 24 * 30);
  $default_datetime = date_convert($default_ts, DATE_UNIX, DATE_DATETIME);
  list($default_date) = explode(' ', $default_datetime);
  
  $default_start_datetime = $default_date .' '. '18:00';
  $default_end_datetime = $default_date .' '. '21:00';

  $fieldset['start_date'] = array(
    '#title' => t('Event Starts'),
    '#field' => $field,
    // TODO: Admins should be able to chose date_select or date_popup on field admin form
    //       Right now we are assuming date_select everywhere
    '#type' => 'date_select',
    '#default_value' => isset($element['#value']['start_date']) ? $element['#value']['start_date'] : $default_start_datetime,
    '#weight' => 2,
    '#required' => 1,
    '#delta'         => $delta,
    // TODO: #date_formet should not be coming from a variable, it should come from a value on field settings form
    //       Not sure how to work granularity in like this code does:
    //       '#date_format' => date_limit_format(date_input_format($element, $field), $field['granularity']),  
    '#date_format' => variable_get('eventbrite_manager_default_date_format', 'F-d-Y h:iA'),
    // TODO: Add something about #date_timezone
    '#date_timezone' => $element['#date_timezone'],
    '#date_text_parts'  => array(),
    '#date_increment'   => 5,
    '#date_year_range'  => '0:+1',
    '#date_label_position' => 'none',
    '#parents' => array($field_name, $delta, 'start_date'),
  );

  // TODO: Add support for date_popup (not just date_select)
  //    date_popup may need #date_titme field filled in on date elements- see date_elements.inc line 338
  
  // This allows date elements to float on the same line
  //   Must be for date_popup as it doesn't do anything with date_select
  // $element['#date_float'] = TRUE;
  
  // Populate end date with start date values
  $fieldset['end_date'] = $fieldset['start_date'];
  $fieldset['end_date']['#title'] = t('Event Ends');
  $fieldset['end_date']['#default_value'] = isset($element['#value']['end_date']) ? $element['#value']['end_date'] : $default_end_datetime;
  $fieldset['end_date']['#weight'] += .5;
  $fieldset['end_date']['#parents'] = array($field_name, $delta, 'end_date');
  
  // TODO: Add user timezone to the mix, currently default is site default
  $fieldset['timezone'] = array(
    '#title' => t('Time Zone'),
    '#type' => 'date_timezone',
    '#options' => date_timezone_names($element['#required']),
    '#weight' => 3,
    '#default_value' => isset($element['#value']['timezone']) ? $element['#value']['timezone'] : variable_get('date_default_timezone_name', ''),
    // Timezone may need to allow blank timezone labels and would then not support being required
    '#required' => 1, //$element['#required'],
    '#parents' => array($field_name, $delta, 'timezone'),
  );

  return $fieldset;
  
}

function _eventbrite_manager_event_widget_process_where($element) {
  $field_name = $element['#field_name'];
	  $delta = $element['#delta'];
//  $fieldset_name = 'where'; Not needed
//  $field = content_fields($field_name, $element['#type_name']); Not needed
  
  // TODO: Add configurability for default date values
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Where'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array(
      'class' => 'eventbrite_cck-new-create',
    ),
  );
  
  $fieldset['venue_info'] = array(
    '#type' => 'item',
    '#value' => t('Venue Handling is not yet implemented'),
  );

  return $fieldset;
  
}

function _eventbrite_manager_event_widget_process_tickets($element) {
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
  $fieldset_name = 'tickets';
//  $field = content_fields($field_name, $element['#type_name']); Not needed
  
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Ticket Information'),
    '#weight' => 4,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array(
      'class' => 'eventbrite_cck-new-create',
    ),
  );
  
  // Provide info about tickets if event has not yet been created
  if (!isset($element['#default_value']['ebid'])) {
    $fieldset['info'] = array(
      '#type' => 'item',
      '#value' => 'You must create the event first before creating any tickets',
      '#weight' => -1,
    );
  }
  
  // Provide info about tickets if event has not yet been created
  if (isset($element['#value']['tickets'][0]['ticket_id'])) {
//  	drupal_set_message('look - '.$element['#value']['tickets'][0]['ticket_id']);
    $fieldset['ticket_id'] = array(
      '#type' => 'value',
      '#value' => $element['#value']['tickets'][0]['ticket_id'],
      '#parents' => array($field_name, $delta, $fieldset_name, 0, 'ticket_id'),
    );
  }
  
  $fieldset['ticket_name'] = array(
    '#title' => t('Ticket Name'),
    '#type' => 'textfield',
    '#default_value' => isset($element['#value'][$fieldset_name][0]['ticket_name']) ? $element['#value'][$fieldset_name][0]['ticket_name'] : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 0,
    '#required' => $element['#required'],
    '#parents' => array($field_name, $delta, $fieldset_name, 0, 'ticket_name'),
    '#description' => t('Examples: Member, Non-member, Student, Early Bird'),
  );
  
    // Don't show status to user on event creation, only on event edit, 
  //   status handling will be automatic on creation
//  if (!empty($defaults['ebid'])) {
  $fieldset['ticket_type'] = array(
    '#title' => t('Ticket Type'),
    '#type' => 'radios',
    '#default_value' => isset($element['#value'][$fieldset_name][0]['ticket_type']) ? $element['#value'][$fieldset_name][0]['ticket_type'] : '',
    '#weight' => 1,
    '#required' => $element['#required'],
    '#options' => array(
      'fixed' => t('Fixed Price'),
      'free' => t('Free'),
      'donation' => t('Donation Format (Attendee can specify payment amount)')
    ),
    '#parents' => array($field_name, $delta, 'tickets', 0, 'ticket_type'),
  );
  
  $fieldset['price'] = array(
    '#title' => t('Price'),
    '#type' => 'textfield',
    '#default_value' => isset($element['#value'][$fieldset_name][0]['price']) ? $element['#value'][$fieldset_name][0]['price'] : '',
    '#weight' => 2,
    '#required' => 0,
    '#size' => 10,
    '#maxlength' => 6,
    '#parents' => array($field_name, $delta, $fieldset_name, 0, 'price'),
    '#description' => t('Only used for Fixed Price tickets'),
  );
  
  $fieldset['quantity'] = array(
    '#title' => t('Quantity Available'),
    '#type' => 'textfield',
    '#default_value' => isset($element['#value'][$fieldset_name][0]['quantity']) ? $element['#value'][$fieldset_name][0]['quantity'] : '',
    '#weight' => 2,
    '#required' => $element['#required'],
    '#size' => 10,
    '#maxlength' => 6,
    '#parents' => array($field_name, $delta, $fieldset_name, 0, 'quantity'),
  );
  
  $fieldset['ticket_description'] = array(
    '#title' => t('Ticket Description'),
    '#type' => 'textarea',
    '#default_value' => isset($element['#value'][$fieldset_name][0]['ticket_description']) ? $element['#value'][$fieldset_name][0]['ticket_description'] : '',
      '#weight' => 5,
    '#parents' => array($field_name, $delta, $fieldset_name, 0, 'ticket_description'),
  );
  
  return $fieldset;
}

function _eventbrite_manager_event_widget_process_discounts($element) {
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
//  $fieldset_name = 'discounts'; Not needed
//  $field = content_fields($field_name, $element['#type_name']); Not needed
  
  // TODO: Add configurability for default date values
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Discount Codes'),
    '#weight' => 6,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array(
      'class' => 'eventbrite_cck-new-create',
    ),
  );
  
  $fieldset['discounts_info'] = array(
    '#type' => 'item',
    '#value' => t('Discount Codes are not yet implemented'),
  );

  return $fieldset;
  
}

function _eventbrite_manager_event_widget_process_payment($element) {
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
//  $fieldset_name = 'discounts'; Not needed
//  $field = content_fields($field_name, $element['#type_name']); Not needed
  
  // TODO: Add configurability for default date values
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Settings'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array(
      'class' => 'eventbrite_cck-new-create',
    ),
  );
  
  $fieldset['payment_info'] = array(
    '#type' => 'item',
    '#value' => t('Payment Settings are not yet implemented'),
  );

  return $fieldset;
  
}


function _eventbrite_manager_event_widget_validate(&$element, &$form_state) {
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];

    // Standard widget validate stuff
  $form_values = $form_state['values'][$field_name][$delta];
  $field = $form_state['#field_info'][$field_name];

  // TODO: See if this is needed
  // This gets called when the field settings get saved,
  //   probably validating the widget in the default settings
  // If the whole field is empty and that's OK, stop now.
  if (empty($element['#post'][$field_name]) && !$element['#required']) {  	
  	drupal_set_message('Empty widget validated in field settings');
    return;
  }
  
  // If ebid and title are both empty then consider the field empty (no validation needed)
  if (empty($element['ebid']['#value']) && empty($element['title']['#value'])) {
//  	drupal_set_message('Empty widget at delta '.$delta);
  	return;
  }

  if (empty($form_values['title'])) {
  	// TODO: Figure out a way for this form_set_error to be put on just the title field for this delta value
    // TODO: add $delta value to error msg
  	form_set_error($field_name, t('The title field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
  }

  // TODO: Set this to an automatic is_numeric handler
  if (!empty($element['data']['capacity']['#value'])) {
  	if (!is_numeric($element['data']['capacity']['#value'])) {
      form_set_error($field_name, t('If the capacity field is filled in it must be numeric', array('!field_label' => $field['widget']['label'])));
  	}
  }

  //-------------------------------------------------
  // TODO: Ticket info validation
  _eventbrite_manager_event_widget_validate_tickets($element, $form_values);

  //----------------------------------------------------
  // Date items and timezone are #required so no need here to look for them
  
  // TODO: Add in a call to date_popup_input_value (this code is from date_select_input_value
  // These are DATETIME strings (ISO dates)
//  $start_date = date_select_input_value($element['when']['start_date']);
//  $end_date = date_select_input_value($element['when']['end_date']);
//  drupal_set_message('timezone value '. $form_values['timezone']);
//  print "h: ";
//  print_r($form_values['timezone']);exit(3);
  
  if ($form_values['start_date'] > $form_values['end_date']) {
    // TODO: add $delta value to error msg
    form_set_error($field_name, t('The end date field must be greater than the start date field in !field_label', array('!field_label' => $field['widget']['label'])));
  }

  // Don't look for further errors if errors are already flagged
  // because otherwise we'll show errors on the nested elements
  // more than once. (?)
  if (form_get_errors()) {
  	return;
  }

  //------------------------------------------------------------------------------  
  // DEPRECATED DATE HANDLING STUFF, don't think we need it
  // _eventbrite_manager_event_widget_validate_date_deprecated($element, $field, $item) {
  
  //------------------------------------------------------------------------------  
  /* THIS WILL BE REMOVED WHEN WE HAVE COMBO WIDGET
  // TODO: Should we check the ebid, if there is one, to see if it is a gettable event
  if (!eventbrite_get('event_get', $event_id)) {
  	form_set_error(
      $element, 
      t(
        'The Eventbrite event id you entered (%ebid) for %field field is not a valid event id in the Eventbrite system',
        array('%ebid' => $event_id, '%field' => $ftitle)
      )
    );
  }
  */
  
}

function _eventbrite_manager_event_widget_validate_tickets($element, $form_values) {
  // Standard widget validate stuff
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
  $field = $form_state['#field_info'][$field_name];

  foreach ($form_values['tickets'] as $index => $ticket) {
    // TODO: Ticket Name is required here
    if (empty($ticket['ticket_name'])) {
  	  // TODO: Figure out a way for this form_set_error to be put on just the title field for this delta value
      // TODO: add $delta value and $index to error msg
  	  form_set_error($field_name, t('The ticket name field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
    }
    
    // TODO: Ticket Type is required here
    if (empty($ticket['ticket_type'])) {
  	  // TODO: Figure out a way for this form_set_error to be put on just the title field for this delta value
      // TODO: add $delta value to error msg
  	  form_set_error($field_name, t('The ticket type field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
    }
    
    // TODO: Ticket Quantity is required here
    if (empty($ticket['quantity']) || !is_numeric($ticket['quantity'])) {
  	  // TODO: Figure out a way for this form_set_error to be put on just the title field for this delta value
      // TODO: add $delta value to error msg
  	  form_set_error($field_name, t('The ticket quantity must be filled in with a numeric value in !field_label', array('!field_label' => $field['widget']['label'])));
    }
  
    // TODO: Price is required here if it's fixed price
    if ($ticket['ticket_type'] == 'fixed') {
      if (empty($ticket['price']) || !is_numeric($ticket['price'])) {
        // TODO: Figure out a way for this form_set_error to be put on just the title field for this delta value
        // TODO: add $delta value to error msg
  	    form_set_error($field_name, t('There must be a price entered for fixd price tickets in !field_label', array('!field_label' => $field['widget']['label'])));
      }
    }    
  }
}

function _eventbrite_manager_event_widget_validate_date_deprecated($element, $field, $item) {
	  // TODO: Check out how date provides alternate error reporting:
  // Unfortunately, due to the fact that much of the processing is already
  // done by the time we get here, it is not possible highlight the field
  // with an error, we just try to explain which element is creating the
  // problem in the error message.
  $parent = $element['#parents'];
  $error_field = array_pop($parent);
  $errors = array();

  // TODO: Add in a call to date_popup_input_value (this coe is from date_select_input_value
  // These are DATETIME strings (ISO dates)
  $from_date = date_select_input_value($element['when']['start_date']);
  $to_date = date_select_input_value($element['when']['end_date']);
    
  // TODO: Check for date validity--most of this is probably not needed
  $from_field = 'start_date';
  $to_field = 'end_date';
  $tz_field = 'timezone';
  $offset_field = 'offset';
  $offset_field2 = 'offset2';
  
  // Convert input dates back to their UTC values and re-format to ISO
  // or UNIX instead of the DATETIME format used in element processing.
  $timezone = !empty($item['timezone']) ? $item['timezone'] : $element['#date_timezone'];
  $timezone_db = date_get_timezone_db($field['tz_handling']);
  // Not sure why timezone would be added back into the form here--doesn't make much sense
  $item[$tz_field] = $timezone;

  // Check date to make offset field
  $from_date = date_make_date($from_date, $timezone);
  $item[$offset_field] = date_offset_get($from_date);    
  $to_date = date_make_date($to_date, $timezone);
  $item[$offset_field2] = date_offset_get($to_date);

  // Get test dates in this timezone, to make sure that dates are valid
  //   (do this before changing the timezone to the db
  $test_from = date_format($from_date, 'r');
  $test_to = date_format($to_date, 'r');

  // Put the dates back into the form as UTC?
  // This can be skipped if timezones are set properly in the widget
  date_timezone_set($from_date, timezone_open($timezone_db));
  date_timezone_set($to_date, timezone_open($timezone_db));
  $item[$from_field] = date_format($from_date, date_type_format($field['type']));
  $item[$to_field] = date_format($to_date, date_type_format($field['type']));
      
  // Test a roundtrip back to the original timezone to catch
  // invalid dates, like 2AM on the day that spring daylight savings
  // time begins in the US.
  date_timezone_set($from_date, timezone_open($timezone));
  date_timezone_set($to_date, timezone_open($timezone));
  if ($test_from != date_format($from_date, 'r')) {
    $errors[] = t('The From date is invalid.');
  }
  if ($test_to != date_format($to_date, 'r')) {
    $errors[] = t('The To date is invalid.');
  }
  
  if (empty($errors)) {
  	// TODO: Not sure why we need to copy this element back into the form
//    form_set_value($element, $item, $form_state);
    return;
  }
  else {
    if ($field['multiple']) {
      form_set_error($error_field, t('There are errors in @field_name value #@delta:', array('@field_name' => $field['widget']['label'], '@delta' => $delta + 1)) . theme('item_list', $errors));
    }
    else {
      form_set_error($error_field, t('There are errors in @field_name:', array('@field_name' => $field['widget']['label'])) . theme('item_list', $errors));      
    }
  }

}