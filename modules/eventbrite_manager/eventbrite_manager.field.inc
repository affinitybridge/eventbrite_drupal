<?php
// $Id$

function _eventbrite_manager_field_settings_form($field) {
  // TODO: There will be settings that go on this form
  //
  // -default date stuff likely
  //       (Maybe we will have to come up with some)  	
    
  // 'form' and 'save' ops may not be needed for this field
  // 
  // Create the form element to be used on the field 
  // settings form. Field settings will be the same for 
  // all shared instances of the same field and should 
  // define the way the value will be stored 
  // in the database.
  $form = array();
  $form['max_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum length'),
    '#default_value' => is_numeric($field['max_length']) ? $field['max_length'] : 255,
    '#required' => FALSE,
        
    // Use #element_validate to validate the settings.
    '#element_validate' => array('_example_length_validate'),
    '#description' => t('The maximum length of the field in characters. Must be a number between 1 and 255'),
  );
  return $form;
}

function _eventbrite_manager_field_settings_save($field) {
  // TODO: There will be settings that go on this form
  //
  // -default date stuff likely
  //       (Maybe we will have to come up with some)  	
    
  // 'form' and 'save' ops may not be needed for this field
  // 
  // Create the form element to be used on the field 
  // settings form. Field settings will be the same for 
  // all shared instances of the same field and should 
  // define the way the value will be stored 
  // in the database.

  // Return an array of the names of the field settings 
  // defined by this module. These are the items that 
  // CCK will store in the field definition
  // and they will be available in the $field array.
  // This should match the items defined in 'form' above.

  return array('max_length');
}

function _eventbrite_manager_field_settings_db_columns($field) {
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

function _eventbrite_manager_field_settings_views_data($field) {
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
function _eventbrite_manager_field_load($node, $field, &$items, $teaser, $page) {
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


function _eventbrite_manager_field_validate(&$node, $field, &$items, $teaser, $page) {
  // Do validation on the field values here. The widget 
  // will do its own validation and you cannot make any 
  // assumptions about what kind of widget has been used, 
  // so don't validate widget values, only field values.

  if (is_array($items)) {
    foreach ($items as $delta => $item) {
      // DO validation here for the field
          
      // If the field is empty just skip, wasn't filled in, that is valid
      if (eventbrite_manager_content_is_empty($item, $field)) {
        return $items;
      }
      if (isset($item['ebid']) && !empty($item['ebid'])) {
        // EB_ID is set, that means all required field elements need to be set as well
        if (empty($item['title'])) {
          form_set_error($field['field_name'], t('The title field is required'));
          return false;
        }
        // description can be empty
        if (empty($item['start_date']) || empty($item['end_date'])) {
          form_set_error($field['field_name'], t('The date fields are required'));
          return false;
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

      // TODO: Add some of this date stuff back in later if there is a need
      /*
      // Don't try to validate if there were any errors before this point since the element won't have been munged back into a date.
      // TODO: Can probably remove the if !form_get_errors() as we're returning if there are any errors
          
      if (!form_get_errors()) {
        foreach ($items as $delta => $item) {
          $process = date_process_values($field);
          foreach ($process as $processed) {
            $error_field = $field['field_name'] .']['. $delta .']['. $processed;
            // Not sure that this next line needs to be in here
            $error_field .= $field['widget']['type'] == 'date_select' ? '][year' : '';
            // $field['todate'] will always be required, these validation routines should be simpler
            if ($processed == 'start_date' && $field['todate']
              && !date_is_valid($item['start_date'], $field['type'], $field['granularity'])
              && (date_is_valid($item['end_date'], $field['type'], $field['granularity']))) {
                form_set_error($error_field, t("A 'From date' date is required for field %field %delta.", array('%delta' => $field['multiple'] ? intval($delta + 1) : '', '%field' => t($field['widget']['label']))));
            }
            if ($processed == 'end_date'
              && $field['todate'] == 'required' && ($field['required']
              && date_is_valid($item['start_date'], $field['type'], $field['granularity'])
              && !date_is_valid($item['end_date'], $field['type'], $field['granularity']))) {
                form_set_error($error_field, t("A 'To date' is required for field %field %delta.", array('%delta' => $field['multiple'] ? intval($delta + 1) : '', '%field' => t($field['widget']['label']))));
            }
          }
        }
      }
      */
          
      /* TODO: Should we checking that ebid is valid?  Widget will not allow for editing of this value, so should be good
      // Check to see that this is a gettable event
      if (!eventbrite_get('event_get', $item['ebid'])) {
        form_set_error($field['field_name'], t('"%ebid" is not a valid event id in the Eventbrite system', array('%ebid' => $item['ebid'])));
        return false;
      }
      */

    }
  }
  return $items;
}

// This is where you make sure that user-provided data is sanitized before being displayed.
function _eventbrite_manager_field_sanitize(&$node, $field, &$items, $teaser, $page) {
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

function _eventbrite_manager_field_presave(&$node, $field, &$items, $teaser, $page) {
  if (is_array($items) && !empty($items)) {
    foreach ($items as $delta => $item) {
      // If the field is empty just skip, wasn't filled in, that is valid
      if (eventbrite_manager_content_is_empty($item, $field)) {
        continue;
      }

      $event = new stdClass();
      $event->title = $item['title'];
      $event->description = $item['description'];
      $event->start_date = $item['start_date'];
      $event->end_date = $item['end_date'];

      // Calculate timezone and offset
      $start_date = date_create($item['start_date'], timezone_open($item['timezone']));
      $items[$delta]['offset'] = $offset = $start_date->getOffset() / 3600;
      $event->timezone = eventbrite_timezone_offset($items[$delta]['offset']);

      if ($item['ebid']) {
        // There is an ebid, which means that an edit/update needs to happen
        $event->event_id = $item['ebid'];
          	
        // TODO: This function should return an event, which will need to be parsed (and cleaned) before save
        if ($event = eventbrite_save('event_update', $event)) {
          drupal_set_message(t('This event was successfully updated'));
          // TODO: parse and clean the event
          // TODO: update field items with values from the server, even if it doesn't matter and they won't change
        }
        else {
          // Save request could not be completed
          // TODO: set a form error here so that the field does not validate (rather than dsm())
          if ($errors = eventbrite_errors()) {
            foreach ($errors as $error_type => $error_msg) {
              drupal_set_message($error_type .': '. $error_msg, 'error');
            }
          }
          drupal_set_message('Could not update the event', 'error');
        }          	
      }
      else {
        // There is no ebid, which means event needs to be created
        if ($event = eventbrite_save('event_new', $event)) {
          drupal_set_message(t('The new event was successfully created - '.$event->id));

          // TODO: Figure out which of these calls are actually putting the variable in and remove cruft
          // TODO: parse and clean the event
          // TODO: Now that eventbrite_save returns an event object and not an event_id, all of these field
          //         items should be updated
          $items[$delta]['ebid'] = $event->id;
          $node_field = $node->$field['field_name'];
          $node_field[$delta]['ebid'] = $event->id;
          $node->$field['field_name'] = $node_field;
        }
        else {
          // TODO: set a form error here so that the field does not validate (rather than dsm())
          drupal_set_message('Could not create the event', 'error');
        }
      }
    }
  }
}
