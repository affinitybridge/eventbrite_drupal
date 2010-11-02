<?php
// $Id$

/**
 * @file
 * Template to display date navigation links.
 *
 * $event
 *   The event object which contains all event fields.
 * 
 */
?>
<div class="eventbrite_cck-event">
  <div class="eventbrite_cck-event-title">
    <h3>
      <?php print($title);?>
    </h3>
  </div>
  
  <div class="eventbrite_cck-event-date">
      <?php print $start_date;?> - <?php print $end_date;?>
  </div>
  
  <div class="eventbrite_cck-event-description">
      <?php print($event->description);?>
  </div>
</div>
