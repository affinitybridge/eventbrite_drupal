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
<hr><hr>
<div class="eventbrite_manager-event">
  <div class="eventbrite_manager-event-title">
    <h3>
      <?php print($title);?>
    </h3>
  </div>
  
  <div class="eventbrite_manager-event-date">
      <?php print $date_range;?>
  </div>
  
  <div class="eventbrite_manager-event-description">
      <?php print($description);?>
  </div>
</div>
