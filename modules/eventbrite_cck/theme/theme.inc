<?php 


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
function theme_eventbrite_cck_formatter_default($element) {
  if ($event = eventbrite_get('event_get', $element['#item']['safe'])) {
	return theme('eventbrite_cck_event', $event);
  }
}

function template_preprocess_eventbrite_cck_event(&$vars) {
  $event = $vars['event'];

  $vars['title'] = $event->title;
  $vars['description'] = $event->description;
  
  $vars['start_date'] = $event->start_date;
  $vars['start_date_ts'] = strtotime($event->start_date);
  $vars['end_date'] = $event->end_date;
  $vars['end_date_ts'] = strtotime($event->end_date);

  // TODO: Add the t() function here for date range
  $vars['date_range'] = format_date($vars['start_date_ts'], 'medium') .' - '. format_date($vars['end_date_ts'], 'medium');
  
}

/**
 * Theme function for 'Event ID' field widget.
 */
function theme_eventbrite_cck_event_id(&$element) {
  return theme('form_element', $element, $element['#children']);
}

/**
 * Theme function for 'Event Title' field widget.
 */
function theme_eventbrite_cck_event_title($element) {
  return theme('form_element', $element, $element['#children']);
}



?>