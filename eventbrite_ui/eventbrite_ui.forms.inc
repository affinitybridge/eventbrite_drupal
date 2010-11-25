<?php

function eventbrite_ui_venue_form($venue) {
  $form = array();

  $form['item'] = array(
    '#type' => 'item',
    '#value' => 'Create or edit a venue with this form',
  );
  
  return $form;
}

function eventbrite_ui_payment_settings_form() {
  $form = array();
  
  $form['accept_paypal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept PayPal payments'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['paypal_email'] = array(
    '#type' => 'textfield',
    '#title' => t('PayPal Email'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['accept_google'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept Google Checkout payments'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['google_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Checkout Merchant ID'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['google_merchant_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Checkout Merchant Key'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['accept_check'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept "Pay by Check" payments'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['instructions_check'] = array(
    '#type' => 'textfield',
    '#title' => 'Instructions to attendees who want to pay by check',
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['accept_cash'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept "Pay with Cash" payments'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['instructions_cash'] = array(
    '#type' => 'textfield',
    '#title' => 'Instructions to attendees who want to pay with cash',
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['accept_invoice'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept "Send an Invoice" payments'),
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );

  $form['instructions_invoice'] = array(
    '#type' => 'textfield',
    '#title' => 'Instructions to attendees who need to be sent an invoice',
//    '#required' => TRUE,
//    '#description' => t('Fill in when necessary.'),
  );
    
  return $form;
}

