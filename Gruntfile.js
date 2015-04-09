iot = require("bindings")("iotivity_nodejs");

module.exports = function(grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
  });

  grunt.registerTask('version', 'IoTivity version', function() {
    grunt.log.write('Running iotivity_nodejs version: ' + iot.version() ).ok();
  });
  
  grunt.registerTask('stop', 'IoTivity stop', function() {
    grunt.log.write('Running iotivity_nodejs stop: ' + iot.stop() ).ok();
  });

  grunt.registerTask('start', 'IoTivity start', function() {
    var handlInfo = function(handlerInfo) {
    };
    
    grunt.log.write('Running iotivity_nodejs start: ' + iot.start(handlInfo) ).ok();
  });
  
  grunt.registerTask('callback', 'IoTivity callback', function() {
    var myCallbackFunction = function(cbparam) {
        grunt.log.write('received callback: ' + cbparam ).ok();
    };
    iot.callback(myCallbackFunction);
    iot.ping();
  });
    
  
}


iot = re...

iot = {
stop: function()...,
registerGetHandler: function()
}...


iot.onGet(
    function(getRequest, response) {
      if(getRequest.resource == "lightbulb") {
        response("turned off");
      }
      ...
    }
  );
