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
  // previously, with the simple field, we just had the id coming in
  //   here, and we fetched from the eventbrite server
  //   This is, we have all the fields we need already...
  //   So why go to Eventbrite?  Why not just use from element?
  //   If DB Cache is not expired then we can definitely just load element
  //   If DB Cache is expired, we will need to grab from eventbrite server
  //   If grab from Eventbrite server matches cache, update cache ts
  //   If grab from Eventbrite server does not match cache, take the time
  //     to unpack the object and insert back into the DB
  // TODO: Make a call to refresh the cache
  
	// TODO: This WILL not work as is because we don't have an EBid
	//  So could write the code do save to eventbrite server now and then have the id
	//    but we need to change this, it should be coming from element rather than event_get
//	foreach ($element as $key => $value) { print "$key\n\n";} exit (4);

	// OK, for the temp being, if there is no ebid on these puppies then call another alternate
	//   function, that will theme without pulling from eventbrite server
	
	// TODO: Sort out this theming crap
  	if (isset($element['#item']['safe_ebid']) && !empty($event['safe_ebid'])) {
      if ($event = eventbrite_get('event_get', $element['#item']['safe_ebid'])) {
	    return theme('eventbrite_cck_event', $event);
      }
  	}
    return theme('eventbrite_manager_event', $element['#item']);
}

function template_preprocess_eventbrite_manager_event(&$vars) {
  $event = $vars['event'];

  $vars['title'] = $event['safe_title'];
  $vars['description'] = $event['safe_description'];

  $vars['start_date'] = $event['safe_start_date'];
  $vars['start_date_ts'] = date_convert($event['safe_start_date'], DATE_DATETIME, DATE_UNIX, $event['timezone']);
  $vars['end_date'] = $event['safe_end_date'];
  $vars['end_date_ts'] = (int)date_convert($event['safe_end_date'], DATE_DATETIME, DATE_UNIX, $event['timezone']);

  // TODO: Add the t() function here
  $vars['date_range'] = format_date($vars['start_date_ts'], 'medium') .' - '. format_date($vars['end_date_ts'], 'medium');
}


?>
