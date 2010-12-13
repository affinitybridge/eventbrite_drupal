<?php
// $Id$

/**
 * @file
 * CCK hook_field and hook_field_settings functions for Eventbrite CCK module
 *
 */

/**
 * Implementation of CCK's hook_field_settings($op = 'database columns').
 */
function _eventbrite_cck_field_settings_db_columns($field) {
  // Define the database storage for this field

  $columns['ebid'] = array(
    'type' => 'int',
    'unsigned' => TRUE,
    'not null' => FALSE,
    'default' => 0,
    'sortable' => TRUE,
    'views' => TRUE,
  );

  $columns['title'] = array(
    'type' => 'varchar',
    'length' => 255,
    'not null' => FALSE,
    'default' => '',
    'sortable' => TRUE,
    'views' => TRUE,
  );

  $columns['status'] = array(
    'type' => 'varchar',
    'length' => 8,
    'default' => '',
    'views' => TRUE,
    'not null' => FALSE,
  );
  $columns['description'] = array(
    'type' => 'text',
    'default' => '',
    'views' => TRUE,
    'not null' => FALSE,
  );

  $columns['start_date'] = array(
    'type' => 'datetime',
    'not null' => FALSE,
    'sortable' => TRUE,
    'views' => TRUE,
  );

  $columns['end_date'] = array(
    'type' => 'datetime',
    'not null' => FALSE,
    'sortable' => TRUE,
    'views' => TRUE,
  );

  $columns['timezone'] = array(
    'type' => 'varchar',
    'length' => 50,
    'not null' => FALSE,
    'sortable' => FALSE,
    'views' => TRUE,
  );

  $columns['offset'] = array(
    'type' => 'int',
    'not null' => FALSE,
    'sortable' => FALSE,
    'views' => FALSE,
  );

  $columns['data'] = array(
    'type' => 'text',
    'serialize' => TRUE,
    'views' => TRUE
  );

  $columns['payment'] = array(
    'type' => 'text',
    'serialize' => TRUE,
    'views' => TRUE
  );

  $columns['tickets'] = array(
    'type' => 'text',
    'serialize' => TRUE,
    'views' => TRUE
  );

  $columns['discounts'] = array(
    'type' => 'text',
    'serialize' => TRUE,
    'views' => TRUE
  );

  return $columns;
}

/**
 * Implementation of CCK's hook_field_settings($op = 'form').
 */
function _eventbrite_cck_field_settings_form($field) {
  // TODO: There will be more settings that go on this form
  //         Maybe default date settings

  $form = array();

  $form['eventbrite'] = array(
    '#type' => 'fieldset',
    '#title' => 'Eventbrite Settings',
  );

  $form['eventbrite']['allow_create_edit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Event Creation and Editing'),
    '#default_value' => $field['allow_create_edit'],
    '#description' => t('Allow for events to be created and edited through this field, with changes pushed to Eventbrite'),
  );
  $form['eventbrite']['allow_load'] = array(
    '#type' => 'checkbox',
    '#title' => t('Pulling events from Eventbrite'),
    '#default_value' => $field['allow_load'],
    '#description' => t('Allow for events previously created on Eventbrite to be loaded into this field'),
  );
  return $form;
}

/**
 * Implementation of CCK's hook_field_settings($op = 'save').
 */
function _eventbrite_cck_field_settings_save($field) {
  return array('allow_create_edit', 'allow_load');
}

/**
 * Implementation of CCK's hook_field_settings($op = 'validate').
 */
function _eventbrite_cck_field_settings_validate($field) {
  if (empty($field['allow_create_edit']) && empty($field['allow_load'])) {
    // TODO: Get the form error set right on this form field
    form_set_error('allow_load', t('You must allow users to either load or create events (or both) to activate an Eventbrite field'));
  }
}

/**
 * Implementation of CCK's hook_field_settings($op = 'views data').
 */
function _eventbrite_cck_field_settings_views_data($field) {
  // TODO: Decide if any changes need to be made to the default $data array created for Views.

  // Start with the $data created by CCK
  // and alter it as needed. The following
  // code illustrates how you would retrieve
  // the necessary data.

  $data = content_views_field_views_data($field);
  $db_info = content_database_info($field);
  $table_alias = content_views_tablename($field);

  // Make changes to $data as needed here.
  //$field_data = $data[$table_alias][$field['field_name'] .'_ebid'];

  // TODO: Create an eventbrite relationship - look at filefield for clues

  /*
  // TODO: Add our special handler for serialized data. - look at filefield functions
  $data[$table_alias][$field['field_name'] .'_data']['field'] = array(
    'title' => $data[$table_alias][$field['field_name'] .'_data']['title'],
    'title short' => $data[$table_alias][$field['field_name'] .'_data']['title short'],
    'field' => $db_info['columns']['data']['column'],
    'table' => $db_info['table'],
    'handler' => 'filefield_handler_field_data',
    'click sortable' => FALSE,
    'access callback' => 'content_access',
    'access arguments' => array('view', $field),
  );

  // Remove handlers that are probably unnecessary.
  unset($data[$table_alias][$field['field_name'] .'_data']['filter']);
  unset($data[$table_alias][$field['field_name'] .'_data']['argument']);
  */

  return $data;
}

/**
 * Implementation of CCK's hook_field($op = 'load').
 */
function _eventbrite_cck_field_load($node, $field, &$items, $teaser, $page) {
  if (empty($items)) {
    return array();
  }
  foreach ($items as $delta => $item) {
    // Despite hook_content_is_empty(), CCK still doesn't filter out
    // empty items from $op = 'load', so we need to do that ourselves.
    if (empty($item['ebid'])) {// || !($file = field_file_load($item['fid']))) {
      $items[$delta] = NULL;
    }
    else {
      $serialized_fields = array('data', 'discounts', 'payment', 'tickets');
      foreach ($serialized_fields as $col) {
        $item[$col] = unserialize($item[$col]);
        // Temporary fix to unserialize data serialized multiple times.
        // See the FileField issue http://drupal.org/node/402860.
        // And the CCK issue http://drupal.org/node/407446.
        while (!empty($item[$col]) && is_string($item[$col])) {
          $item[$col] = unserialize($item[$col]);
        }
      }
      $items[$delta] = $item;
    }
  }

  return array($field['field_name'] => $items);
}

/**
 * Implementation of CCK's hook_field($op = 'validate').
 */
function _eventbrite_cck_field_validate(&$node, $field, &$items, $teaser, $page) {
  // Do validation on the field values here. The widget
  // will do its own validation and you cannot make any
  // assumptions about what kind of widget has been used,
  // so don't validate widget values, only field values.

  if (is_array($items)) {
    foreach ($items as $delta => $item) {
      // DO validation here for each field

      // If the field is empty just skip, wasn't filled in, that is valid
      if (eventbrite_cck_content_is_empty($item, $field)) {
        return $items;
      }
      if (isset($item['ebid']) && !empty($item['ebid'])) {
        // EB_ID is set, that means all required field elements need to be set as well if editing is allowed
        if ($field['allow_create_edit']) {
          if (empty($item['title'])) {
            form_set_error($field['field_name'], t('The title field is required'));
            return FALSE;
          }
          // description can be empty
          if (empty($item['start_date']) || empty($item['end_date'])) {
            form_set_error($field['field_name'], t('The date fields are required'));
            return FALSE;
          }
        }
      }
      else {
        /* Not sure about this one
        // EB ID is blank, that means everything else should be blank
         * Should we set an error if EB is blank and other fields have values?
        if (!empty($item['title'])) {
          form_set_error($field['field_name'], t('No ebid, so title should be empty'));
          return false;
       	}
        */
      }

      // TODO: Add some date validation stuff back in later if there is a need
    }
  }
  return $items;
}

/**
 * Implementation of CCK's hook_field($op = 'sanitize').
 */
function _eventbrite_cck_field_sanitize(&$node, $field, &$items, $teaser, $page) {
  // Get the 'data' column stored by CCK into an array. This is necessary
  // for Views, which doesn't call the "load" $op and to fix an issue with
  // CCK double-serializing data.
  // See the FileField issue http://drupal.org/node/402860.
  // And the CCK issue http://drupal.org/node/407446.
  while (!empty($items[$delta]['data']) && is_string($items[$delta]['data'])) {
    $items[$delta]['data'] = unserialize($items[$delta]['data']);
  }

  // TODO: this should be replaced to sanitize calls to the actual field item keys.
  foreach ($items as $delta => $item) {
    // TODO: Should not have to test that $item is non-empty,
    //       empty fields should not be in sanitize data
    if ($item) {
      foreach ( $item as $col => $dat ) {
        switch ($col) {
          case 'data':
          case 'discounts':
          case 'payment':
          case 'tickets':
            // Get the 'data' column stored by CCK into an array. This is necessary
            // for Views, which doesn't call the "load" $op and to fix an issue with
            // CCK double-serializing data.
            // See the FileField issue http://drupal.org/node/402860.
            // And the CCK issue http://drupal.org/node/407446.
            while (!empty($items[$delta][$col]) && is_string($items[$delta][$col])) {
              $items[$delta][$col] = unserialize($items[$delta][$col]);
            }
            // TODO: maybe add some processing for serialized values
            break;

          default:
            $items[$delta]['safe_' . $col ] = check_plain($item[ $col ]);
        }
      }
    }
  }
}

/**
 * Implementation of CCK's hook_field($op = 'presave').
 */
function _eventbrite_cck_field_presave(&$node, $field, &$items, $teaser, $page) {
  if (is_array($items) && !empty($items)) {
  	// Loop through multiple form values
    foreach ($items as $delta => $item) {
      // If the field is empty just skip, wasn't filled in, that is valid
      if (eventbrite_cck_content_is_empty($item, $field)) {
        continue;
      }
      $new_event = NULL;

      // If user either has no permissions or if it's a new field and user is pulling from EB
      if (!$field['allow_create_edit'] || $item['eventbrite_new'] == 'load') {
        // TODO: ops for load
        if ($item['load_ebid']) {
          // TODO: Possibly check here if the event id has changed (probably we should refresh the event either way)
          // if ($item['ebid'] && ($item['ebid'] != $item['load_ebid'])) {

          $new_event_id = $item['load_ebid'];

        }
        else {
          drupal_set_message('Error, this field should not have been validated to this point.', 'error');
        }
      }
      else {
        //TODO: ops for create/edit
        $event = _eventbrite_cck_populate_event_from_form($item);
        $items[$delta]['offset'] = $event->tz_offset;

        // Check if there is an event_id set in event
        if (isset($event->event_id)) {
          // There is an ebid, which means the event needs to be updated
          if ($new_event_id = eventbrite_save('event_update', $event)) {
            drupal_set_message(t('This event was successfully updated on the Eventbrite server. '));

          }
          else {
            // Save request could not be completed
            // TODO: set a form error here so that the field does not validate (rather than dsm())
            // TODO: Not sure errors should be printed here at all
            if ($errors = eventbrite_errors()) {
              foreach ($errors as $error_type => $error_msg) {
                drupal_set_message($error_type .': '. $error_msg, 'error');
              }
            }
            drupal_set_message(t('This event could not be updated on the Eventbrite server'), 'error');
          }
        }
        else {
          // There is no ebid, which means the event needs to be created
          if ($new_event_id = eventbrite_save('event_new', $event)) {
            drupal_set_message(t('The new event was successfully created on the Eventbrite server. '));
          }
          else {
            // Create request could not be completed
            // TODO: set a form error here so that the field does not validate (rather than dsm())
            drupal_set_message('This event could not be created on the Eventbrite server', 'error');
          }
        }
      }

      // Get fresh event data from the server, whether the event was loaded, updated, or created      
      if ($new_event = eventbrite_get('event_get', $new_event_id)) {
      	// Update field values with values from the Eventbrite server
        $items[$delta]['ebid'] = $new_event->id;
        $items[$delta]['title'] = $new_event->title;
        $items[$delta]['status'] = $new_event->status;
        $items[$delta]['description'] = $new_event->description;
        $items[$delta]['start_date'] = $new_event->start_date;
        $items[$delta]['end_date'] = $new_event->end_date;
        // TODO: These timezones coming from Eventbrite server are DEPRECATED & will not work in D7
        $items[$delta]['timezone'] = $new_event->timezone;
      	
        // Update the ticket settings
        // TODO: Allow for multiple ticket processing
        _eventbrite_cck_field_presave_ticket($items[$delta]['tickets'][0], $new_event, $item[load_ebid]);

        // Update the payment settings
        _eventbrite_cck_field_presave_payment_settings($items[$delta]['payment'], $new_event, $item[load_ebid]);
      }
      else {
        // Could not load that new event from the Eventbrite server
        drupal_set_message('Could not load that event from the eventbrite server', 'error');
        drupal_set_message('This field could not be updated on the eventbrite server', 'error');
        // at this point not sure what we should do...maybe just leave the field as is
        // if we do not explicitly change the ebid then nothing should matter
      }
    }
  }
}



/*
 * This function massages the payment setting before the ticket is saved on the EB server and in the DB
 */
function _eventbrite_cck_field_presave_ticket(&$ticket_item, $event, $load_ebid = 0) {
  if ($ticket_item['ticket_toggle']) {
    // We only need to make this API call if user has changed ticket info
    $ticket = _eventbrite_cck_populate_ticket_from_form($ticket_item);

    // If this ticket has already been created
    if (isset($ticket->id)) {
      // There is a ticket id, which means that an edit/update needs to happen
      if ($ticket_id = eventbrite_save('ticket_update', $ticket)) {
        drupal_set_message(t('This ticket was successfully updated on the Eventbrite server'));
      }
      else {
        // Save request could not be completed
        // TODO: clear the ticket values--the event has not been submitted
        if ($errors = eventbrite_errors()) {
          foreach ($errors as $error_type => $error_msg) {
            drupal_set_message($error_type .': '. $error_msg, 'error');
          }
        }
        drupal_set_message('Could not update the ticket', 'error');
      }
    }
    else {
      // There is no ticket id, which means ticket needs to be created
      $ticket->event_id = $event->id;

      if ($ticket_id = eventbrite_save('ticket_new', $ticket)) {
        drupal_set_message(t('The new ticket was successfully created on the Eventbrite server.'));

        // Set ticket id in item that will be saved to DB
        // TODO: Figure these fields out
        // Other ticket fields will be autosaved in tickets
        $ticket_item['ticket_id'] = $ticket_id;
      }
      else {
        // TODO: set a form error here so that the field does not validate (rather than dsm())
        // TODO: clear ticket values or something
        drupal_set_message('Could not create the ticket', 'error');
      }
    }
    // The call to create/update ticket just returns ticket id w/ confirmation
    //   And Eventbrite does not have a get ticket API call yet, so nothing to stuff back into the database
  }
}

/*
 * This function massages the payment setting before the event is saved on the EB server and in the DB
 */
function _eventbrite_cck_field_presave_payment_settings(&$payment_item, $event, $load_ebid = 0) {
  if ($payment_item['payment_toggle']) {
    // We only need to make this API call if user has changed payment settings info
    $payment_settings = _eventbrite_cck_populate_payment_settings_from_form($payment_item);
    $payment_settings->event_id = $event->id;

    if ($message = eventbrite_save('payment_update', $payment_settings)) {
      drupal_set_message(t('The payment settings were successfully saved on the Eventbrite server: !method', array('!method' => $message)));
    }
    else {
      // Save request could not be completed
      // TODO: Payment settings were not saved...what should we do here?
      if ($errors = eventbrite_errors()) {
        foreach ($errors as $error_type => $error_msg) {
          drupal_set_message($error_type .': '. $error_msg, 'error');
        }
      }
      drupal_set_message('Could not update the payment settings on the Eventbrite server', 'error');
    }          	
  }
  else {
    // TODO: Payment settings for loaded events need to be editable
    
  	// This is load_ebid field within payment is a temporary fix to mark payment settings that we don't know
    // The payment settings form does not get built so payment just goes into the DB with load_ebid
    if ($load_ebid) {
      $payment_item = array(
        'load_ebid' => $load_ebid,
      );
    }

    // Check if empty flag is set--if it is, then clear these form values before saving
    if (isset($payment_item['payment_empty'])) {
      // These payment settings should be empty, so clear form values (which are just the defaults) before save
      $payment_item = array();
    }
  }
}

/*
 * This function pulls event info from the form results and builds an event object for base Eventbrite module
 */
function _eventbrite_cck_populate_event_from_form($item) {
  $event = new stdClass();

  if ($item['ebid']) {
    $event->event_id = $item['ebid'];
  }
  $event->title = $item['title'];
  $event->description = $item['description'];
  $event->status = $item['status'];

  $event->start_date = $item['start_date'];
  $event->end_date = $item['end_date'];

  $start_date = date_create($item['start_date'], timezone_open($item['timezone']));
  $event->tz_offset = $start_date->getOffset() / 3600;
  $event->timezone = eventbrite_timezone_offset($event->tz_offset);

  return $event;
}

/*
 * This function pulls ticket info from the form results and builds a ticket object for base Eventbrite module
 */
function _eventbrite_cck_populate_ticket_from_form($item) {
  $ticket = new stdClass();

  // Add ticket data to request
  if (isset($item['ticket_id'])) {
    $ticket->id = $item['ticket_id'];
  }
  $ticket->name = $item['ticket_name'];
  $ticket->quantity = $item['quantity'];
  if (!empty($item['ticket_description'])) {
    $ticket->description = $item['ticket_description'];
  }
  switch ($item['ticket_type']) {
    case 'fixed':
      $ticket->price = $item['price'];
      $ticket->is_donation = 0;
      break;
    case 'free':
      $ticket->price = 0.00;
      $ticket->is_donation = 0;
      break;
    case 'donation':
      $ticket->price = '';
      $ticket->is_donation = 1;
      break;
    default:
      $ticket->price = $item['price'];
      $ticket->is_donation = 0;
      break;
  }
  return $ticket;
}

/*
 * This function pulls payment settings info from the form results and builds a payment settings object for base Eventbrite module
 */
function _eventbrite_cck_populate_payment_settings_from_form($item) {
  $payment = new stdClass();

  $payment->accept_paypal = isset($item['accept_paypal']) ? 1 : 0;
  $payment->paypal_email = $item['paypal_email'];
  $payment->accept_google = isset($item['accept_google']) ? 1 : 0;
  $payment->google_merchant_id = $item['google_merchant_id'];
  $payment->google_merchant_key = $item['google_merchant_key'];
  $payment->accept_check = isset($item['accept_check']) ? 1 : 0;
  $payment->instructions_check = $item['instructions_check'];
  $payment->accept_cash = isset($item['accept_cash']) ? 1 : 0;
  $payment->instructions_cash = $item['instructions_cash'];
  $payment->accept_invoice = isset($item['accept_invoice']) ? 1 : 0;
  $payment->instructions_invoice = $item['instructions_invoice'];

  return $payment;
}