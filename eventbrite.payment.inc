<?php
// $Id$

/**
 * @file
 * Defines payment handling functions
 */

function eventbrite_payment_settings_form($default_values = array()) {
  $form = array();
  
  $form['accept_paypal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept PayPal payments'),
    '#default_value' => isset($default_values['accept_paypal']) ? $default_values['accept_paypal'] : 0,
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['paypal_email'] = array(
    '#type' => 'textfield',
    '#title' => t('PayPal Email'),
    '#default_value' => isset($default_values['paypal_email']) ? $default_values['paypal_email'] : '',
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['accept_google'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept Google Checkout payments'),
    '#default_value' => isset($default_values['accept_google']) ? $default_values['accept_google'] : 0,
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['google_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Checkout Merchant ID'),
    '#default_value' => isset($default_values['google_merchant_id']) ? $default_values['google_merchant_id'] : '',
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['google_merchant_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Checkout Merchant Key'),
    '#default_value' => isset($default_values['google_merchant_key']) ? $default_values['google_merchant_key'] : '',
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['accept_check'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept "Pay by Check" payments'),
    '#default_value' => isset($default_values['accept_check']) ? $default_values['accept_check'] : 0,
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['instructions_check'] = array(
    '#type' => 'textfield',
    '#title' => 'Instructions to attendees who want to pay by check',
    '#default_value' => isset($default_values['instructions_check']) ? $default_values['instructions_check'] : '',
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['accept_cash'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept "Pay with Cash" payments'),
    '#default_value' => isset($default_values['accept_cash']) ? $default_values['accept_cash'] : 0,
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['instructions_cash'] = array(
    '#type' => 'textfield',
    '#title' => 'Instructions to attendees who want to pay with cash',
    '#default_value' => isset($default_values['instructions_cash']) ? $default_values['instructions_cash'] : '',
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['accept_invoice'] = array(
    '#type' => 'checkbox',
    '#title' => t('Accept "Send an Invoice" payments'),
    '#default_value' => isset($default_values['accept_invoice']) ? $default_values['accept_invoice'] : 0,
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );

  $form['instructions_invoice'] = array(
    '#type' => 'textfield',
    '#title' => 'Instructions to attendees who need to be sent an invoice',
    '#default_value' => isset($default_values['instructions_invoice']) ? $default_values['instructions_invoice'] : '',
    '#attributes' => array('class' => 'eventbrite-payment-settings'),
  );
  
  return $form;
}

// Temporary function
// TODO $form_errors wil have to turn into form_set_errors()
function eventbrite_payment_settings_errors($values) {
  $form_errors = array();
  
  if ($values['accept_paypal']) {
    // If using PayPay check that paypal email is entered
    if (!valid_email_address($values['paypal_email'])) {
      // TODO: Does this need to actually verified that there is a PayPal account registered to this user
      $form_errors['paypal_email'] = t('Please provide a valid Paypal email.');
    }
  }
  
  if ($values['accept_google']) {
    // If using Google Checkout check that Merchant ID is a 10 or 15 digit number
    if (!preg_match('/^([0-9]{10}|[0-9]{15})$/', $values['google_merchant_id'])) {
      $form_errors['google_merchant_id'] = t('To use Google Checkout you need to enter a 10 or 15 digit Google Checkout Merchant ID.');
    }
    // If using Google Checkout check that Merchant Key is a 22 character string
    if (!preg_match('/^(.{22})$/', $values['google_merchant_key'])) {
      $form_errors['google_merchant_id'] = t('To use Google Checkout you need to enter a 22 character Google Checkout Merchant Key.');
    }
  }
  return $form_errors;
}

/*
 * Default Payment Settings Form
 */
function eventbrite_payment_default_settings() {
  $default_values = variable_get('eventbrite_default_payment_settings', array());

  $form = eventbrite_payment_settings_form($default_values);
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Update default settings',
  );

  return $form;
}

/*
 * Default Payment Settings Form Validate
 */
function eventbrite_payment_default_settings_validate($form, &$form_state) {
  $form_values = $form_state['values'];

  $form_errors = eventbrite_payment_settings_errors($form_values);
  foreach ($form_errors as $form_element => $error_msg) {
    form_set_error($form_element, $error_msg);
  }
}

/*
 * Default Payment Settings Form Submit
 */
function eventbrite_payment_default_settings_submit($form, &$form_state) {
  variable_set('eventbrite_default_payment_settings', $form_state['values']);
}