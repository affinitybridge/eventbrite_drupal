<?php
// $Id$

/**
 * @file
 * Theme functions for Eventbrite CCK module
 *
 */

/**
 * Theme function for 'Event Manager' field widget.
 */
function theme_eventbrite_cck_event_widget(&$element) {
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
function theme_eventbrite_cck_formatter_default($element) {
  // TODO: Should empty fields be coming in here for theming?
  //       Do we really need to be checking for ebid here?

  // Do not format this (empty) field if ebid is not set
  if ($element['#item']['ebid']) {
    return theme('eventbrite_cck_event', $element['#item']);
  }
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
function theme_eventbrite_cck_formatter_teaser($element) {
  // TODO: Should empty fields be coming in here for theming?
  //       Do we really need to be checking for ebid here?

  // Do not format this (empty) field if ebid is not set
  if ($element['#item']['ebid']) {
    return theme('eventbrite_cck_event_teaser', $element['#item']);
  }
}

function theme_eventbrite_cck_formatter_ticket_embed($element) {
  if ($element['#item']['ebid']) {
    $source_url = 'http://www.eventbrite.com/tickets-external?eid='. $element['#item']['ebid'] .'&ref=etckt';
    $height = '192';
    $output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" width="100%" vspace="0" hspace="0" marginheight="5" marginwidth="5" scrolling="auto" allowtransparency="true"></iframe></div>';
    return $output;
  }
}
function theme_eventbrite_cck_formatter_registration_embed($element) {
  if ($element['#item']['ebid']) {
    $source_url = 'http://www.eventbrite.com/event/'. $element['#item']['ebid'] .'&ref=eweb';
    $height = '550';
    $output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" width="100%" vspace="0" hspace="0" marginheight="5" marginwidth="5" scrolling="auto" allowtransparency="true"></iframe></div>';
    return $output;
  }
}
function theme_eventbrite_cck_formatter_countdown_embed($element) {
  if ($element['#item']['ebid']) {
    $source_url = 'http://www.eventbrite.com/countdown-widget?eid='. $element['#item']['ebid'];
    $height = '259';
    $output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" marginheight="0" marginwidth="0" scrolling="no"></iframe></div>';
    return $output;
  }
}
function theme_eventbrite_cck_formatter_calendar_embed($element) {
  if ($element['#item']['ebid']) {
    $source_url = 'http://www.eventbrite.com/calendar-widget?eid='. $element['#item']['ebid'];
    $height = '333';
    $output .= '<div style="width:100%; text-align:left;"><iframe  src="'. $source_url .'" frameborder="0" height="'. $height .'" marginheight="0" marginwidth="0" scrolling="no"></iframe></div>';
    return $output;
  }
}

/*
 * Preprocess call
 */
function template_preprocess_eventbrite_cck_event(&$vars) {
  $event = $vars['event'];

  $vars['title'] = $event['safe_title'];
  $vars['description'] = $event['safe_description'];

  $vars['start_date'] = $event['safe_start_date'];
  $vars['start_date_ts'] = date_convert($event['safe_start_date'], DATE_DATETIME, DATE_UNIX, $event['timezone']);
  $vars['end_date'] = $event['safe_end_date'];
  $vars['end_date_ts'] = (int)date_convert($event['safe_end_date'], DATE_DATETIME, DATE_UNIX, $event['timezone']);

  // TODO: Add the t() function here for date range
  $vars['date_range'] = format_date($vars['start_date_ts'], 'medium') .' - '. format_date($vars['end_date_ts'], 'medium');

  $vars['attend_link'] = l(
    t('Attend this Event'),
    'http://www.eventbrite.com/event/'. $event['safe_ebid'] .'?ref=elink',
    array('attributes' => array('target' => '_blank'))
  );
}
