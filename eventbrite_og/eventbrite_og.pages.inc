<?php
// $Id$

function eventbrite_og_venue_form($form_state, $venue_id = 0) {
//  $payment_settings_form = eventbrite_ui_payment_settings_form();
//  return system_settings_form($payment_settings_form);
//drupal_set_message('yar'.$yar);
  og_set_group_context($group_node);
	
// TODO: payment settings form?  THis won't do!
//  module_load_include('inc', 'eventbrite_ui', 'eventbrite_ui.forms');
  //  $form = eventbrite_ui_payment_settings_form();
  
  return $form;
}

function eventbrite_og_venue_list($group_node) {
  og_set_group_context($group_node);

  module_load_include('inc', 'eventbrite', 'eventbrite.venue');
  
  return eventbrite_venue_list();
}

function eventbrite_og_default_payment_settings($form_state, $group_node) {
  og_set_group_context($group_node);

  module_load_include('inc', 'eventbrite', 'eventbrite.payment');
  $form = eventbrite_ui_payment_settings_form();
  
  return $form;
}

