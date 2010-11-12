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

function theme_eventbrite_cck_formatter_ticket_embed($element) {
  if ($event = eventbrite_get('event_get', $element['#item']['safe'])) {
  	$source_url = 'http://www.eventbrite.com/tickets-external?eid='. $element['#item']['safe'] .'&ref=etckt';
  	$height = '192';
  	$output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" width="100%" vspace="0" hspace="0" marginheight="5" marginwidth="5" scrolling="auto" allowtransparency="true"></iframe></div>';
  	return $output;
  }
}
function theme_eventbrite_cck_formatter_registration_embed($element) {
  if ($event = eventbrite_get('event_get', $element['#item']['safe'])) {
  	$source_url = 'http://www.eventbrite.com/event/'. $element['#item']['safe'] .'&ref=eweb';
  	$height = '550';
  	$output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" width="100%" vspace="0" hspace="0" marginheight="5" marginwidth="5" scrolling="auto" allowtransparency="true"></iframe></div>';
  	return $output;
  }
}
function theme_eventbrite_cck_formatter_countdown_embed($element) {
  if ($event = eventbrite_get('event_get', $element['#item']['safe'])) {
  	$source_url = 'http://www.eventbrite.com/countdown-widget?eid='. $element['#item']['safe'];
  	$height = '259';
  	$output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" marginheight="0" marginwidth="0" scrolling="no"></iframe></div>';
  	return $output;
  }
}
function theme_eventbrite_cck_formatter_calendar_embed($element) {
  if ($event = eventbrite_get('event_get', $element['#item']['safe'])) {
    $source_url = 'http://www.eventbrite.com/calendar-widget?eid='. $element['#item']['safe'];
    $height = '333';
    $output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" marginheight="0" marginwidth="0" scrolling="no"></iframe></div>';
    return $output;
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
  
  $vars['attend_link'] = l(
    t('Attend this Event'),
    'http://www.eventbrite.com/event/'. $event->link .'?ref=elink', 
    array('attributes' => array('target' => '_blank'))
  );
  
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