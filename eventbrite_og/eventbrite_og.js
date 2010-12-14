Drupal.behaviors.eventBrite = function (context) {
  $('input.eventbrite_og-account').click(function() {
    if ($(this).attr('value') == 'eb_og_subuser' ) {
      $('#edit-eb-subuser-email-wrapper').show();
      $('#edit-eb-user-key-wrapper').hide();
    }
    else if ($(this).attr('value') == 'eb_og_custom' ) {
      $('#edit-eb-subuser-email-wrapper').hide();
      $('#edit-eb-user-key-wrapper').show();
    }
    else {
      $('#edit-eb-subuser-email-wrapper').hide();
      $('#edit-eb-user-key-wrapper').hide();
    }
  });
	 
  if ($(this).attr('value') != 'eb_og_subuser' ) {
    $('#edit-eb-subuser-email-wrapper').hide();
  }
  if ($(this).attr('value') != 'eb_og_custom' ) {
    $('#edit-eb-user-key-wrapper').hide();
  }
 
};
