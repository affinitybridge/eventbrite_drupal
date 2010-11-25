The Eventbrite module is currently being refactored into a 2.x branch that will expose
  the entire Eventbrite API to Drupal modules.

Changes from 1.x

In 2.x the base Eventbrite module is an API module that exposes Eventbrite API methods to
  other Drupal modules.  Included with the Eventbrite module will be several sub modules
  to offer different UI options

Eventbrite Base Module (API)

Calls made to the server  are cached to cut down on unnecessary requests as well as 
  decreasing page render time.
  
Exposed API functions (callable by UI submodules)

eventbrite_get($op, $id = NULL);
  Params: $op  The remote API method at Eventbrite server
               Current supported $op values are:
                 user_list_events
                 event_get
          $id  Optional param for the remote API calls that take an id key

eventbrite_save($op, $object);
  Params: $op  The remote API method at Eventbrite server
               Current supported $op values are:
                 event_new
          $id  Object to be saved

Exposed Hooks

hook_eventbrite_request_alter(&$request);
  This hook is currently not implemented.  It will allow UI submodules to alter the
    request before it goes to the Eventbrite server.  This will allow modules to
    alter the maximum cache time as well as change the user making the request.

UI Submodules

Eventbrite CCK

This implements Eventbrite events as CCK fields.  It will be the complete solution for
  event integration.  Create events, tickets, venues, etc., all from within Drupal.
  This module makes heavy use of the API.

Eventbrite OG (not yet implemented, will depend on Eventbrite CCK)

This module will add Organic Groups Integration.  It will create an Eventbrite subuser
  for each OG group and allow group admins to manage events for the group under that
  subuser.  It will be glue code that integrates Eventbrite Complete with OG.
