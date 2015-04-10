iot = require("bindings")("iotivity_wrapper_native");

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
    function handleRequest(req) {
      console.log("Got a request for: " + req.resource);
      req.respond();
    }

    console.log("Booting iotivity");

    iot.start(handleRequest);

    //don't exit
    setInterval(function() {  }, 3000 );

  });

}
