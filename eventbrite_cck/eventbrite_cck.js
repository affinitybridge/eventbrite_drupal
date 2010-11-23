Drupal.behaviors.eventBrite = function (context) {
  // Toggle between load event and create event
  $('input.eventbrite_cck-new-toggle').click(function() {
    if ($(this).attr('value') == 'load' ) {
      $('fieldset.eventbrite_cck-new-load').show();
      $('fieldset.eventbrite_cck-new-create').hide();
    }
    else if ($(this).attr('value') == 'create' ) {
      $('fieldset.eventbrite_cck-new-load').hide();
      $('fieldset.eventbrite_cck-new-create').show();
    }
  });
  
  // Set ticket input fields to disabled (readonly does not work for some field elements)
  //   If ticket update does not get checked these fields will get re-enabled on node submit
  disableTicketFields(true);
  $('input.eventbrite_cck-ticket-toggle').attr('disabled', false);
  
  // On submit, re-enable all disabled fields so that values get submitted
  $('#edit-submit').click(function() {
    disableTicketFields(false);
  });
  
  // Trying to toggle whether ticket fields are editable
  $('input.eventbrite_cck-ticket-toggle').click(function() {
    disableTicketFields(false);
    $('input.eventbrite_cck-ticket-toggle').attr('disabled', true);
  });

  function disableTicketFields($disabled) {
    $('input.eventbrite_cck-ticket-input').attr('disabled', $disabled);
    $('textarea.eventbrite_cck-ticket-input').attr('disabled', $disabled);
    $('input.eventbrite_cck-ticket-toggle').attr('disabled', $disabled);
  }
};
