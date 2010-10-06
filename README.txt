The Eventbrite module is currently being refactored into a 2.x branch that will expose
  the entire Eventbrite API to Drupal modules.  The 2.x branch (currently in a github repo)
  implements the entirety of the 1.0 feature set.  We are aiming to get an Alpha release
  into the contrib CVS account soon for people to evaluate.

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

Eventbrite Complete (not yet implemented)

This is the complete solution.  Create events, tickets, venues, etc., all from within
  Drupal.  This module will most likely make the event a CCK field that can be attached
  to any node type.  This module will make heavy use of the API and give site admins the
  ability to do almost everything.

Eventbrite CCK (not yet implemented)

This is a much lighter weight UI module.  Events would have to be managed on the 
  Eventbrite site, and Drupal users would have the ability to attach one of the site's
  events to a node through a simple CCK wdiget.

Eventbrite OG (not yet implemented, will depend on Eventbrite Complete)

This module will add Organic Groups Integration.  It will create an Eventbrite subuser
  for each OG group and allow group admins to manage events for the group under that
  subuser.  It will be glue code that integrates Eventbrite Complete with OG.

Eventbrite UI

This is a very simple UI module that shows list of events and allows for very simple event editing.
  It exists to help test the API.  It is unclear at this point whether this is a submodule that will
  make any release.

Eventbrite Blocks

This UI submodule supports the legacy functionality in 1.0.  It creates 5 blocks for each event in the
  user's event list.  While it implements the entirety of the 1.0 feature set, there is currently no
  upgrade path written.
         
