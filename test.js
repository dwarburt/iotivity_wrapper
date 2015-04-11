iot = require("bindings")("iotivity_wrapper_native");
    function handleRequest(req) {
      console.log("Got a request for: " + req.resource);
      req.params[0] = "Value for p1";
      req.respond();
    }

    console.log("Booting iotivity");

    iot.start(handleRequest, "a/genericdevice");


