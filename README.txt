The Eventbrite module is currently being refactored into a 2.x branch that will expose
  the entire Eventbrite API to Drupal modules.  The 2.x branch currently implements
  the entirety of the 1.x-dev feature set

Changes from 1.x

The The 2.x branch currently implements the entirety of the 1.x feature set.
  In 2.x the base Eventbrite module is an API module that allows other modules to call
  the Eventbrite API.  To get the block functionality from 1.x you must enable 
  the Eventbrite Blocks module.  There is currently no upgrade step written--
  so when you upgrade you will lose your current block settings.

Eventbrite API

The 2.x Eventbrite base module provides access to the Eventbrite API.  Calls made to the server 
  are cached to cut down on unnecessary requests as well as decreasing page render time.

The Eventbrite base module also enables a hook:  hook_eventbrite_request_alter($request).
  This hook will allow other modules to alter information passed in the request.

Eventbrite Sub Modules

Currently the only submodule calling Eventbrite is Eventbrite Blocks, 
  which implements the block functionality from 1.x.  
  More submodules are planned, including CCK and Organic Groups integration.