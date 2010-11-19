Drupal.behaviors.eventBrite = function (context) {
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
};
