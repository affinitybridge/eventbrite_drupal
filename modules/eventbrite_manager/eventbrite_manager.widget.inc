<?php
// $Id$

function _eventbrite_manager_event_widget_process($element, $edit, &$form_state, $form) {
  // TODO: This was from date, not sure it's needed
  if (isset($element['#access']) && empty($element['#access'])) {
    return $element;
  }

  // Make sure ebid gets set (it seems to require special handling)
  $defaults = $element['#value'];
  if (isset($element['#default_value']['ebid'])) {
  	$defaults['ebid'] = $element['#default_value']['ebid'];
  }
  $field_name = $element['#field_name'];
  
  $field = content_fields($field_name, $element['#type_name']);
  $delta = $element['#delta'];
  
  $element['ebid'] = array(
    '#type' => 'value',
    '#value' => $defaults['ebid'],
  );

  $element['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $defaults['title'],
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 0,
    '#required' => $element['#required'],
  );

  $element['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $defaults['description'],
    '#weight' => 1,
  );
  
  // TODO: Add configurability for default date values
  
  // For now, set default event date 30 days in the future
  $default_ts = time() + (60 * 60 * 24 * 30);
  $default_datetime = date_convert($default_ts, DATE_UNIX, DATE_DATETIME);
  list($default_date) = explode(' ', $default_datetime);
  
  $default_start_datetime = $default_date .' '. '18:00';
  $default_end_datetime = $default_date .' '. '21:00';
  
  $element['start_date'] = array(
    '#title' => t('Start date'),
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
  );

  // TODO: Add support for date_popup (not just date_select)
  //    date_popup may need #date_titme field filled in on date elements- see date_elements.inc line 338
  
  // This allows date elements to float on the same line
  //   Must be for date_popup as it doesn't do anything with date_select
  // $element['#date_float'] = TRUE;
  
  // Populate end date with start date values
  $element['end_date'] = $element['start_date'];
  $element['end_date']['#title'] = t('End date');
  $element['end_date']['#default_value'] = isset($element['#value']['end_date']) ? $element['#value']['end_date'] : $default_end_datetime;
  $element['end_date']['#weight'] += .5;
  
  // TODO: Add user timezone to the mix, currently default is site default
  $element['timezone'] = array(
    '#type' => 'date_timezone',
    '#options' => date_timezone_names($element['#required']),
    '#weight' => 3,
    '#default_value' => isset($element['#value']['timezone']) ? $element['#value']['timezone'] : variable_get('date_default_timezone_name', ''),//$default_start_datetime,
    // Timezone may need to allow blank timezone labels and would then not support being required
    '#required' => 1, //$element['#required'],
  );

  // placeholder.. will be serialized into the data column. this is a place for widgets
  // to put additional data.
  $element['data'] = array(
    '#tree' => 'true',
    '#access' => !empty($item['ebid']),
  );

  $element['data']['capacity'] = array(
    '#type' => 'textfield',
    '#title' => t('Capacity'),
    '#value' => isset($item['data']['capacity']) ? $item['data']['capacity'] : '',
    '#size' => 10,
    '#maxlength' => 6,
  );

  
  
  
  
  // Date had this, may as well leave it in
  // Make sure field info will be available to the validator which does not get the values in $form.
  $form_state['#field_info'][$field['field_name']] = $field;

  return $element;
}

function _eventbrite_manager_event_widget_validate(&$element, &$form_state) {
  // Standard widget validate stuff
  $form_values = $form_state['values'];
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
  $field = $form_state['#field_info'][$field_name];
  
  // TODO: See if this is needed
  // If the whole field is empty and that's OK, stop now.
  if (empty($element['#post'][$field_name]) && !$element['#required']) {
  	drupal_set_message('LOOKY LOOKY NOTE: This chunk got called');
    return;
  }
  
  // If ebid and title are both empty then consider the field empty (no validation needed)
  if (empty($element['ebid']['#value']) && empty($element['title']['#value'])) {
  	drupal_set_message('Empty widget at delta '.$delta);
  	return;
  }
  
  if (empty($element['title']['#value'])) {
  	// TODO: Figure out a way for this form_set_error to be put on just the title field for this delta value
    // TODO: add $delta value to error msg
  	form_set_error($field_name, t('The title field is required for event submission in !field_label', array('!field_label' => $field['widget']['label'])));
  }

  // Date items and timezone are #required so no need here to look for them
  
  // TODO: Add in a call to date_popup_input_value (this code is from date_select_input_value
  // These are DATETIME strings (ISO dates)
  $start_date = date_select_input_value($element['start_date']);
  $end_date = date_select_input_value($element['end_date']);
  
  if ($start_date > $end_date) {
      // TODO: no real need to call form_set_value()
//      form_set_value($element['end_date'], 'end_date', $form_state);
    // TODO: add $delta value to error msg
    form_set_error($field_name, t('The end date field must be greater than the start date field in !field_label', array('!field_label' => $field['widget']['label'])));
  }

  // Don't look for further errors if errors are already flagged
  // because otherwise we'll show errors on the nested elements
  // more than once. (?)
  if (form_get_errors()) {
  	return;
  }

  // TODO: probably shouldn't return here, should do more date processing
  //       just making sure that, for now, we don't need the form_set_value()s for any processing
  return;

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
  $from_date = date_select_input_value($element['start_date']);
  $to_date = date_select_input_value($element['end_date']);
    
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
  
  /*
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

