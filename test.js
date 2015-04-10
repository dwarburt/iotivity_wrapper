iot = require("bindings")("iotivity_wrapper_native");
    function handleRequest(req) {
      console.log("Got a request for: " + req.resource);
      req.respond();
    }

    console.log("Booting iotivity");

    iot.start(handleRequest);


