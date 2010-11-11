<?php 

/**
 * Theme function for 'Event Manager' field widget.
 */
function theme_eventbrite_manager_event_widget(&$element) {
  return theme('form_element', $element, $element['#children']);
}


/**
 * Theme function for 'default' example field formatter.
 * 
 * $element['#item']: the sanitized $delta value for the item,
 * $element['#field_name']: the field name,
 * $element['#type_name']: the $node->type,
 * $element['#formatter']: the $formatter_name,
 * $element'#node']: the $node,
 * $element['#delta']: the delta of this item, like '0',
 * 
 */
function theme_eventbrite_manager_formatter_default($element) {
  // When event_manager field is loaded, an event_get call is made
  //   on load op and data is refreshed as needed
  
  // TODO: This WILL not work as is because we don't have an EBid
  // TODO: Not sure when we would be coming in here without having an eventbrite ID
  //         This may have just applied earlier on in the process before event creation

  return theme('eventbrite_manager_event', $element['#item']);
}

/*
 * Preprocess call
 */
function template_preprocess_eventbrite_manager_event(&$vars) {
  $event = $vars['event'];

  $vars['title'] = $event['safe_title'];
  $vars['description'] = $event['safe_description'];

  $vars['start_date'] = $event['safe_start_date'];
  $vars['start_date_ts'] = date_convert($event['safe_start_date'], DATE_DATETIME, DATE_UNIX, $event['timezone']);
  $vars['end_date'] = $event['safe_end_date'];
  $vars['end_date_ts'] = (int)date_convert($event['safe_end_date'], DATE_DATETIME, DATE_UNIX, $event['timezone']);

  // TODO: Add the t() function here for date range
  $vars['date_range'] = format_date($vars['start_date_ts'], 'medium') .' - '. format_date($vars['end_date_ts'], 'medium');
}


?>
