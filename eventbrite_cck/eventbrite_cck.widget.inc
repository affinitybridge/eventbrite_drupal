<?php
// $Id$

/**
 * @file
 * CCK hook_field and hook_field_settings functions for Eventbrite CCK module
 *
 */

/**
 * Implementation of CCK's hook_widget_process.
 */
function _eventbrite_cck_event_widget_process($element, $edit, &$form_state, $form) {
  // If #access is set and FALSE then don't show the widget
  // TODO: Verify whether or not we need this, perhaps if access is false it won't matter
  if (isset($element['#access']) && empty($element['#access'])) {
    return $element;
  }

  $field_name = $element['#field_name'];
  $field = content_fields($field_name, $element['#type_name']);
  $delta = $element['#delta'];
  $form_values = $form_state['values'][$field_name][$delta];

  $ebid = isset($element['#default_value']['ebid']) ? $element['#default_value']['ebid'] : 0;

  $element['ebid'] = array(
    '#type' => 'value',
    '#value' => $ebid,
  );

  // This js is needed throughout the form
  drupal_add_js(drupal_get_path('module', 'eventbrite_cck')  .'/eventbrite_cck.js', 'module');

  if (empty($ebid)) {
    // Show source widget if this event field is empty
    if ($field['allow_create_edit'] && $field['allow_load']) {

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
        '#attributes' => array('class' => 'eventbrite_cck-new-toggle'),
      );
    }
  }

  if ($field['allow_load']) {
    if (!$ebid || !$field['allow_create_edit']) {

      $element['load'] = array(
        '#type' => 'fieldset',
        '#title' => t('Load an event from Eventbrite'),
        '#weight' => -5,
        '#collapsible' => FALSE,
        '#attributes' => array(
          'class' => 'eventbrite_cck-new-load',
        ),
      );

      if ($ebid) {
        $edit_link = l(
          t('Edit this Event at Eventbrite'),
          'http://www.eventbrite.com/edit?eid='. $ebid,
          array('attributes' => array('target' => '_blank'))
        );
        $element['load']['edit_link'] = array(
          '#type' => 'markup',
          '#value' => $edit_link,
          '#weight' => -10,
        );
      }

      // If there are events for this user at Eventbrite, show a select box
      if ($events_options = eventbrite_get_events_options()) {
        $element['load']['load_ebid'] = array(
          '#title' => t( 'Eventbrite Event' ),
          '#type' => 'select',
          '#options' => $events_options,
          '#default_value' => $ebid,
          '#weight' => -0,
          '#parents' => array($field_name, $delta, 'load_ebid'),
        );

      }
      else {
        $element['load']['no_events'] = array(
          '#type' => 'item',
          '#value' => t('There are currently no events created in this account.'),
        );
      }

    }
  }

  if ($field['allow_create_edit']) {
    $fieldset_class = empty($ebid) ? 'eventbrite_cck-new-create ' : 'eventbrite_cck-new-edit ';
    
    $element['event_details'] = _eventbrite_cck_event_widget_process_details($element);
    $element['event_details']['#attributes']['class'] = $fieldset_class;
    $element['when'] = _eventbrite_cck_event_widget_process_when($element);
    $element['when']['#attributes']['class'] = $fieldset_class;
//    $element['where'] = _eventbrite_cck_event_widget_process_where($element);
    $element['tickets'] = _eventbrite_cck_event_widget_process_tickets($element);
    $element['tickets']['#attributes']['class'] = $fieldset_class;
    $element['payment'] = _eventbrite_cck_event_widget_process_payment($element, $fieldset_class);
    $element['payment']['#attributes']['class'] = $fieldset_class;
    // TODO: Add discounts back in
    /*
    $element['discounts'] = _eventbrite_cck_event_widget_process_discounts($element);
    $element['discounts']['#attributes']['class'] = $fieldset_class;
    */
    // Hide the select box unless this form is being shown again and it was selected

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
  }

  // Put $field into $form_state for validate() function
  $form_state['#field_info'][$field['field_name']] = $field;

  return $element;
}

/**
 * Process event details
 */
function _eventbrite_cck_event_widget_process_details($element, $ebid = '') {
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Event Details'),
    '#weight' => -2,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
//    '#attributes' => array(
//      'class' => 'eventbrite_cck-new-create',
//    ),
  );

  if (!empty($ebid)) {
    $edit_link = l(
      t('Edit this Event at Eventbrite'),
      'http://www.eventbrite.com/edit?eid='. $ebid,
      array('attributes' => array('target' => '_blank'))
    );
    $fieldset['edit_link'] = array(
      '#type' => 'markup',
      '#value' => $edit_link,
      '#weight' => -10,
    );
  }

  $fieldset['title'] = array(
    '#title' => t('Event Title'),
    '#type' => 'textfield',
    '#default_value' => isset($element['#value']['title']) ? $element['#value']['title'] : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 0,
    '#required' => $element['#required'],
    '#parents' => array($field_name, $delta, 'title'),
  );

  // TODO: Add & properly handle more status values
  $fieldset['status'] = array(
    '#title' => t('Status'),
    '#type' => 'radios',
    '#default_value' => isset($element['#value']['status']) ? $element['#value']['status'] : 'draft',
    '#weight' => 1,
    '#required' => 1,
    '#options' => array('draft' => t('Draft'), 'live' => t('Live')),
    '#parents' => array($field_name, $delta, 'status'),
  );

  module_load_include('inc', 'eventbrite', 'eventbrite.venue');
  $fieldset['venue_id'] = array(
    '#type' => 'select',
    '#title' => t('Venue'),
    '#default_value' => isset($element['#value']['venue_id']) ? $element['#value']['venue_id'] : '',
    '#options' => eventbrite_venue_options(),
    '#parents' => array($field_name, $delta, 'venue_id'),
  );
  
  $fieldset['description'] = array(
    '#title' => t('Event Description'),
    '#type' => 'textarea',
    '#default_value' => isset($element['#value']['description']) ? $element['#value']['description'] : '',
    '#weight' => 1,
    '#parents' => array($field_name, $delta, 'description'),
  );

  return $fieldset;
}

/**
 * Process dates
 */
function _eventbrite_cck_event_widget_process_when($element) {
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
    '#attributes' => array('class' => 'eventbrite_cck-new-create'),
  );

  // TODO: Add configurability for default date
  //       For now, set default event date 30 days in the future
  $default_ts = time() + (60 * 60 * 24 * 30);
  $default_datetime = date_convert($default_ts, DATE_UNIX, DATE_DATETIME);
  list($default_date) = explode(' ', $default_datetime);

  $default_start_datetime = $default_date .' '. '18:00';
  $default_end_datetime = $default_date .' '. '21:00';

  // TODO: Add date_popup capability to dates
  //    date_popup may need #date_titme field filled in on date elements- see date_elements.inc line 338
  // $element['#date_float'] = TRUE;
  // TODO: Massage timezone
  // TODO: Add the variable filling in date_format to the settings page
  
  $fieldset['start_date'] = array(
    '#title' => t('Event Starts'),
    '#field' => $field,
    '#type' => 'date_select',
    '#default_value' => isset($element['#value']['start_date']) ? $element['#value']['start_date'] : $default_start_datetime,
    '#weight' => 2,
    '#required' => 1,
    '#delta'         => $delta,
    '#date_format' => variable_get('eventbrite_cck_default_date_format', 'F-d-Y h:iA'),
    '#date_text_parts'  => array(),
    '#date_increment'   => 5,
    '#date_year_range'  => '0:+1',
    '#date_label_position' => 'none',
    '#parents' => array($field_name, $delta, 'start_date'),
  );

  // Populate end date with start date values
  $fieldset['end_date'] = $fieldset['start_date'];
  $fieldset['end_date']['#title'] = t('Event Ends');
  $fieldset['end_date']['#default_value'] = isset($element['#value']['end_date']) ? $element['#value']['end_date'] : $default_end_datetime;
  $fieldset['end_date']['#weight'] += .5;
  $fieldset['end_date']['#parents'] = array($field_name, $delta, 'end_date');

  $timezone_list = date_timezone_names(TRUE);
  if (isset($element['#value']['timezone'])) {
    $timezone = $element['#value']['timezone'];
    if (!in_array($timezone, $timezone_list)) {
      // If the current timezone is not in this array, add it
      $timezone_list = array($timezone => $timezone) + $timezone_list;
    }
  }
  else {
    // TODO: check if user has a timezone, if so, use it
    $timezone = variable_get('date_default_timezone_name', '');
  }

  // Do not set the type to 'date_timezone' here from the date_api module,
  //   it will overwrite Eventbrite's deprecated Olson names
  $fieldset['timezone'] = array(
    '#title' => t('Time Zone'),
    '#type' => 'select',
    '#options' => $timezone_list,
    '#weight' => 3,
    '#default_value' => $timezone,
    '#required' => 1,
    '#parents' => array($field_name, $delta, 'timezone'),
  );

  return $fieldset;
}

/**
 * Process venue
 */
function _eventbrite_cck_event_widget_process_where($element) {
  // TODO: Add venue handling

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Where'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
//    '#attributes' => array(
//      'class' => 'eventbrite_cck-new-create eventbrite_cck-where',
//    ),
  );

  $fieldset['venue_info'] = array(
    '#type' => 'item',
    '#value' => t('Venue Handling is not yet implemented'),
  );

  return $fieldset;
}

/**
 * Process tickets.
 */
function _eventbrite_cck_event_widget_process_tickets($element) {
  // TODO: Allow processing of more than one ticket

  // Get field values for tickets, if they've been populated
  $element_values = isset($element['#value']['tickets'][0]) ? $element['#value']['tickets'][0] : array();

  $parents = array($element['#field_name'], $element['#delta'], 'tickets', 0);
  
  $ticket_id = isset($element_values['ticket_id']) ? $element_values['ticket_id'] : '';

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Ticket Information'),
    '#weight' => 4,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // TODO: This is a bandaid since we don't have a way yet to edit tickets for events loaded from the server
  if (isset($element_values['load_ebid'])) {
    $fieldset['load_ebid_msg'] = array(
      '#type' => t('item'),
      '#value' => t('This event has been loaded from the Eventbrite server.  Editing its tickets is not supported at this time'),
    );

    $fieldset['load_ebid'] = array(
      '#type' => 'value',
      '#value' => $element_values['ebid'],
    );

    // Do not build the rest of the editing form for Tickets  	
    // TODO: Perhaps it would be better to just return here (without $fieldset) while tickets are not editable
    return $fieldset;
  }
  
  
  // This checkbox will toggle and allow us to know if ticket values need to be updated
  $fieldset['ticket_toggle'] = array(
    '#type' => 'checkbox',
    '#title' => ($ticket_id) ? t('Update ticket values') : t('Create a ticket for this event.'),
    '#default_value' => 0,
    '#weight' => -10,
    '#description' => ($ticket_id) ? t('Check this box to update ticket values for this event') : t('A ticket must be created for the event to go Live'),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-toggle'),
    '#parents' => array_merge($parents, array('ticket_toggle')),
  );

  
  // Ticket ID if ticket already exists
  if ($ticket_id) {
    $fieldset['ticket_id'] = array(
      '#type' => 'hidden',
      '#value' => $element_values['ticket_id'],
    '#parents' => array_merge($parents, array('ticket_id')),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-id'),
    );
  }

  $fieldset['ticket_name'] = array(
    '#title' => t('Ticket Name'),
    '#type' => 'textfield',
    '#default_value' => isset($element_values['ticket_name']) ? $element_values['ticket_name'] : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 0,
//    '#required' => $element['#required'],
    '#parents' => array_merge($parents, array('ticket_name')),
    '#description' => t('Examples: Member, Non-member, Student, Early Bird'),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-input'),
    '#prefix' => '<div class="eventbrite_cck-ticket-wrapper">',
  );

  $fieldset['ticket_type'] = array(
    '#title' => t('Ticket Type'),
    '#type' => 'radios',
    '#default_value' => isset($element_values['ticket_type']) ? $element_values['ticket_type'] : '',
    '#weight' => 1,
//    '#required' => $element['#required'],
    '#options' => array(
      'fixed' => t('Fixed Price'),
      'free' => t('Free'),
      'donation' => t('Donation Format Attendee can specify payment amount')
    ),
    '#parents' => array_merge($parents, array('ticket_type')),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-input'),
  );

  $fieldset['price'] = array(
    '#title' => t('Price'),
    '#type' => 'textfield',
    '#default_value' => isset($element_values['price']) ? $element_values['price'] : '',
    '#weight' => 2,
    '#size' => 10,
    '#maxlength' => 6,
    '#parents' => array_merge($parents, array('price')),
    '#description' => t('Only used for Fixed Price tickets'),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-input'),
  );

  $fieldset['quantity'] = array(
    '#title' => t('Quantity Available'),
    '#type' => 'textfield',
    '#default_value' => isset($element_values['quantity']) ? $element_values['quantity'] : '',
    '#weight' => 2,
    '#size' => 10,
    '#maxlength' => 6,
    '#parents' => array_merge($parents, array('quantity')),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-input'),
  );
  $fieldset['ticket_description'] = array(
    '#title' => t('Ticket Description'),
    '#type' => 'textarea',
    '#default_value' => isset($element_values['ticket_description']) ? $element_values['ticket_description'] : '',
    '#weight' => 5,
    '#parents' => array_merge($parents, array('ticket_description')),
    '#attributes' => array('class' => 'eventbrite_cck-ticket-input'),
    '#suffix' => '</div>',
  );

  return $fieldset;
}

/**
 * Process discount codes.
 * 
 * TODO: discount codes are not currently supported
 */
function _eventbrite_cck_event_widget_process_discounts($element) {
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Discount Codes'),
    '#weight' => 6,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $fieldset['discounts_info'] = array(
    '#type' => 'item',
    '#value' => t('Discount Codes are not yet implemented'),
  );

  return $fieldset;
}

/**
 * Process payment settings.
 */
function _eventbrite_cck_event_widget_process_payment($element, $fieldset_class) {
  // Get field values for payment settings, if they've been populated
  $element_values = isset($element['#value']['payment']) ? $element['#value']['payment'] : array();

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Settings'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
//    '#attributes' => array('class' => $fieldset_class .' eventbrite_cck-payment-settings'),
  );

  // TODO: This is a bandaid since we don't have a way yet to edit payment settings for events loaded form the server
  if (isset($element_values['load_ebid'])) {
    $fieldset['load_ebid_msg'] = array(
      '#type' => t('item'),
      '#value' => t('This event has been loaded from the Eventbrite server.  Editing its payment settings is not supported at this time'),
    );

    $fieldset['load_ebid'] = array(
      '#type' => 'value',
      '#value' => $element_values['ebid'],
    );

    // Do not build the rest of the editing form for Payment Settings  	
    // TODO: Perhaps it would be better to just return here (without $fieldset) while pay ment settings are not editable
    return $fieldset;
  }

  // This checkbox will toggle and allow us to know if ticket values need to be updated
  $fieldset['payment_toggle'] = array(
    '#type' => 'checkbox',
    '#title' => 'Update payment settings',
    '#default_value' => 0,
    '#weight' => -10,
    '#description' => 'Update payment settings for this event',
    '#attributes' => array('class' => 'eventbrite_cck-payment-toggle'),
  );

  // Load include file with payment functions
  module_load_include('inc', 'eventbrite', 'eventbrite.payment');

  if (empty($element_values)) {
    // Payment settings have not been defined for this event
    $fieldset['payment_toggle']['#title'] = t('Enter payment settings for this event.');
    $fieldset['payment_toggle']['#description'] = t('You have not configured any payment settings for this event.');
    
    // Fill default settings into the form since no payment settings have been configured
    $default_values = variable_get('eventbrite_default_payment_settings', array());
    $payment_settings_form = eventbrite_payment_settings_form($default_values);

    // Add a flag so that the field knows that payment settings have not been defined
    // Must be a hidden field since CCK does not seem to work with '#type' => 'value')
    $fieldset['payment_empty'] = array(
      '#type' => 'hidden',
      '#value' => 1,
      '#attributes' => array('class' => 'eventbrite_cck-payment-empty'),
    );
  }
  else {
    // Payment Settings have already been created for this event
    $payment_settings_form = eventbrite_payment_settings_form($element_values);
  }
  
  return $fieldset + $payment_settings_form;
}

/**
 * Implementation of CCK's hook_widget_validate.
 */
function _eventbrite_cck_event_widget_validate(&$element, &$form_state) {
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];

  // Standard widget validate stuff
  $form_values = $form_state['values'][$field_name][$delta];
  $field = $form_state['#field_info'][$field_name];

  // This runs for validation of the the default value on the field settings form
  // TODO: Add more appropriate default value on the field settings form
  // If the whole field is empty and it's not required, stop now.
  if (empty($element['#post'][$field_name]) && !$element['#required']) {
    return;
  }

  // Get ebid if there is one already set to the field
  $ebid = $element['ebid']['#value'];

  // If this is a new event instance and the source widget has not even been selected
  if (!$ebid && !isset($form_values['eventbrite_new'])) {
    // There is no event here, the field is empty
    if ($element['#required']) {
      // Throw an error if the field is required
      form_set_error($field_name, t('Event information is required for !field_label', array('!field_label' => $field['widget']['label'])));
    }
    // No need to keep validating this empty field
    return;
  }

  // If the load event widget is being used for this field, then validate it
  // Either the field is explicitly set to be loaded, or else load permissions are all the user has
  if ($form_values['eventbrite_new'] == 'load' || !$field['allow_create_edit']) {
    _eventbrite_cck_event_widget_validate_load_ebid($element, $form_values, $ebid);
    // That's all the validation needed for the load widget
    return;
  }

  // At this point we know there's an Eventbrite edit/create form widget

  // If title is empty and element is required, throw an error
  if (empty($form_values['title'])) {
    if ($element['#required'] || $ebid) {
      form_set_error($field_name, t('The title field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
    }
    // If there's no title then this event is empty, don't bother validating the rest of the fields
    // TODO: Should we return here?  Or process more in the form?
    return;
  }

  // TODO: Add capacity back in
  /*
  // TODO: Set this to an automatic is_numeric handler
  if (!empty($element['data']['capacity']['#value'])) {
  	if (!is_numeric($element['data']['capacity']['#value'])) {
      form_set_error($field_name, t('If the capacity field is filled in it must be numeric', array('!field_label' => $field['widget']['label'])));
  	}
  }
  */

  // Validate tickets
  _eventbrite_cck_event_widget_validate_tickets($element, $form_values);
  
  // Validate payment settings  
  _eventbrite_cck_event_widget_validate_payment($element, $form_values);

  // Validate event dates
  // TODO: Set dates to required (they are probalby already required and selected on the form)
  // TODO: Add processing for data_popups (date_popup_input_value -- code from date_select_input_value)
  //       These are DATETIME strings (ISO dates)
  //       $start_date = date_select_input_value($element['when']['start_date']);
  //       $end_date = date_select_input_value($element['when']['end_date']);
  if ($form_values['start_date'] > $form_values['end_date']) {
    form_set_error($field_name, t('The end date field must be greater than the start date field in !field_label', array('!field_label' => $field['widget']['label'])));
  }
}

/**
 * Validate loaded event.
 * 
 * TODO: set a better form error (label and element)
 */
function _eventbrite_cck_event_widget_validate_load_ebid($element, $form_values, $ebid = 0) {
  // If load_ebid is empty and element is required, throw an error
  if (empty($form_values['load_ebid'])) {
    if ($element['#required'] || $ebid) {
      form_set_error($element['#field_name'], t('The Event select field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
    }
    return;
  }

  if ($form_values['load_ebid'] != $ebid) {
    // Eventbrite ID has changed, or is new, so we'll need to verify the ebid with the server.
    if (!eventbrite_get('event_get', $form_values['load_ebid'])) {
      form_set_error(
        $element['#field_name'],
        t(
          'The Eventbrite event id you entered (%ebid) for field is not a valid event id in the Eventbrite system',
          array('%ebid' => $form_values['load_ebid'])
        )
      );
    }
  }
}

/**
 * Validate tickets.
 * 
 * TODO: set a better form error (label and element)
 */
function _eventbrite_cck_event_widget_validate_tickets($element, $form_values) {
  $field_name = $element['#field_name'];

  foreach ($form_values['tickets'] as $index => $ticket) {
    if ($ticket['ticket_toggle']) {
      // Ticket Name is required here
      if (empty($ticket['ticket_name'])) {
        form_set_error($field_name, t('The ticket name field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
      }
      
      // Ticket Type is required
      if (empty($ticket['ticket_type'])) {
        form_set_error($field_name, t('The ticket type field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
      }

      // Ticket Quantity is required
      if (empty($ticket['quantity']) || !is_numeric($ticket['quantity'])) {
        form_set_error($field_name, t('The ticket quantity must be filled in with a numeric value in !field_label', array('!field_label' => $field['widget']['label'])));
      }

      // Price is required here if ticket type is fixed price
      if ($ticket['ticket_type'] == 'fixed') {
        if (empty($ticket['price']) || !is_numeric($ticket['price'])) {
          form_set_error($field_name, t('There must be a price entered for fixd price tickets in !field_label', array('!field_label' => $field['widget']['label'])));
        }
      }
    }
    else {
      if (empty($ticket['ticket_id'])) {
        // Ticket id is not set, no ticket exists
        // Better make sure that this ticket status wasn't set to live
        if ($form_values['status'] == 'live') {
          form_set_error($field_name, t('You must create a ticket for this event if you want it to be Live in !field_label', array('!field_label' => $field['widget']['label'])));
        }
      }
    }
  }
}

/**
 * Validate payment settings.
 * 
 * TODO: set a better form error (label and element)
 * TODO: Check that at least one method of payment is set (otherwise it's an error)
 */
function _eventbrite_cck_event_widget_validate_payment($element, $form_values) {
  // Only validate if payment_toggle is set
  if ($form_values['payment']['payment_toggle']) {
    // Validation of payment form handled by Eventbrite base module
    $form_errors = eventbrite_payment_settings_errors($form_values['payment']);
    foreach ($form_errors as $form_element => $error_msg) {
      form_set_error($element['#field_name'], $error_msg);
    }
  }
}
