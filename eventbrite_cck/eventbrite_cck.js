Drupal.behaviors.eventBrite = function (context) {
  // Toggle between load event and create/edit event widgets
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
  if ($('input:checked.eventbrite_cck-new-toggle').val() != 'load') {
    $('fieldset.eventbrite_cck-new-load').hide();
  }
  if ($('input:checked.eventbrite_cck-new-toggle').val() != 'create') {
    $('fieldset.eventbrite_cck-new-create').hide();
  }
 
	 
  // On submit, re-enable all disabled fields so that values get submitted
  $('#edit-submit').click(function() {
	reEnableTicketFields();
    reEnablePaymentFields();
  });
  
  // Set ticket input fields to disabled (readonly does not work for some field elements)
  //   If ticket update does not get checked these fields will get re-enabled on node submit

  
  updateTicketFields();
  
  // Trying to toggle whether payment fields are editable
  $('input.eventbrite_cck-ticket-toggle').click(function() {
    updateTicketFields();
  });
  
  
  
  // Trying to toggle whether ticket fields are editable
//  $('input.eventbrite_cck-ticket-toggle').click(function() {
//    disableTicketFields(false);
//    $('input.eventbrite_cck-ticket-toggle').attr('disabled', true);
//  });

//  function disableTicketFields($disabled) {
//    $('input.eventbrite_cck-ticket-input').attr('disabled', $disabled);
//    $('textarea.eventbrite_cck-ticket-input').attr('disabled', $disabled);
//    $('input.eventbrite_cck-ticket-toggle').attr('disabled', $disabled);
//  }

  function updateTicketFields() {
	    if ($('input.eventbrite_cck-ticket-toggle').attr('checked')) {
//	    	eventbrite_cck-ticket-field-wrapper
	    	$('.eventbrite_cck-ticket-input').attr('disabled', false);
	        $('.eventbrite_cck-ticket-wrapper').css('display', 'block');
//	        $('textarea.eventbrite_cck-ticket-input').parent().parent().parent().css('display', 'block');
//	    	$('.eventbrite_cck-ticket-input').attr('disabled', false);
	//        $('.eventbrite_cck-ticket-input').parent().css('display', 'block');
	  //      $('textarea.eventbrite_cck-ticket-input').parent().parent().parent().css('display', 'block');
	       // $('textarea.eventbrite_cck-ticket-input').attr('disabled', false);
	       // $('textarea.eventbrite_cck-ticket-input').parent().css('display', 'block');

	        $('input.eventbrite_cck-ticket-toggle').attr('disabled', 'disabled');
	      }
	      else {
	        if ($('input.eventbrite_cck-ticket-id').length) {
	          // This ticket has already been created, show fields disabled
		      $('.eventbrite_cck-ticket-input').attr('disabled', 'disabled');
	        }
	        else {
	          // Disable payment settings if they have been created
			      $('.eventbrite_cck-ticket-wrapper').css('display', 'none');
//		      $('.eventbrite_cck-ticket-input').parent().css('display', 'none');
//		      $('textarea.eventbrite_cck-ticket-input').parent().parent().parent().css('display', 'none');
	        }
	        $('input.eventbrite_cck-ticket-toggle').attr('disabled', false);
	      }
  }
  function reEnableTicketFields() {
    $('input.eventbrite_cck-ticket-input').attr('disabled', false);
    $('textarea.eventbrite_cck-ticket-input').attr('disabled', false);
    $('input.eventbrite_cck-ticket-toggle').attr('disabled', false);
  }
  
  // Set payment input fields to disabled (readonly does not work for some field elements)
  //   If payment update does not get checked these fields will get re-enabled on node submit
  updatePaymentFields();
  
  // Trying to toggle whether payment fields are editable
  $('input.eventbrite_cck-payment-toggle').click(function() {
    updatePaymentFields();
  });

  function reEnablePaymentFields() {
	$('input.eventbrite_cck-payment-toggle').attr('disabled', false);    
  }
  function updatePaymentFields() {
    if ($('input.eventbrite_cck-payment-toggle').attr('checked')) {
      $('input.eventbrite-payment-settings').parent().css('display', 'block');
      $('input.eventbrite-payment-settings').attr('disabled', false);
      $('input.eventbrite_cck-payment-toggle').attr('disabled', 'disabled');
    }
    else {
      if ($('input.eventbrite_cck-payment-empty').length) {
        // Hide payment settings if none have been created
        $('input.eventbrite-payment-settings').parent().css('display', 'none');
      }
      else {
        // Disable payment settings if they have been created
        $('input.eventbrite-payment-settings').attr('disabled', 'disabled');
      }
      $('input.eventbrite_cck-payment-toggle').attr('disabled', false);
    }
  }
};
