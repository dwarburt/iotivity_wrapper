iot = require("bindings")("iotivity_wrapper_native");
module.exports = iot;
console.log("got iot: " + iot.version());