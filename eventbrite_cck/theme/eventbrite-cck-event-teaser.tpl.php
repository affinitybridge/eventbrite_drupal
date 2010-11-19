<?php
// $Id$

/**
 * @file
 * Template to display an event teaser.
 *
 * $event
 *   The event object which contains all event fields.
 * 
 */
?>
<hr><hr>
<div class="eventbrite-cck-event">
  <div class="eventbrite-cck-event-title">
    <h3>
      <?php print($title);?>
    </h3>
  </div>
  
  <div class="eventbrite-cck-event-date">
      <?php print $date_range;?>
  </div>
  
  <div class="eventbrite-cck-event-description">
      <?php print($description);?>
  </div>

  <div class="eventbrite-cck-event-attend">
      <?php print($attend_link);?>
  </div>
</div>
  