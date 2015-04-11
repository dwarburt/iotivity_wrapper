iotivity_wrapper
=============
### Wrapper module for iotivity's native csdk

This module wraps the iotivity csdk and provides the basic functions
of the csdk to nodejs.

To use: 

1. have iotivity checked out and built on your system.
2. set the environment variable IOTIVITY_HOME to the root of iotivity
3. create a node project if you don't have one (npm init).
4. npm install --save git+https://github.com/dwarburt/iotivity_wrapper.git

**N.B. tested against the plugfest-1 branch.**

prereqs: git and node and iotivity installed.


Example

>    iot = require("bindings")("iotivity_wrapper_native");
>    function handleRequest(req) {
>      console.log("Got a request for: " + req.resource);
>      req.respond();
>    }
>
>    console.log("Booting iotivity");
>
>    iot.start(handleRequest);
>

