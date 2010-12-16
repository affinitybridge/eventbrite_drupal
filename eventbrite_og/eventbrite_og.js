Drupal.behaviors.eventBrite = function (context) {
  $('input.eventbrite_og-enabled').click(function() {
    if ($(this).attr('value') == 0) {
      $('#edit-eventbrite-subuser-email-wrapper').hide();
      $('#edit-eventbrite_og-user-key-wrapper').hide();
    }
    else {
      $('#edit-eventbrite-subuser-email-wrapper').show();
      $('#edit-eventbrite_og-user-key-wrapper').show();
    }
  });

  if ($('input:checked.eventbrite_og-enabled').val() == 0) {
    $('#edit-eventbrite-subuser-email-wrapper').hide();
    $('#edit-eventbrite_og-user-key-wrapper').hide();
  } 
};
