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
    var handlInfo = function(handlerInfo) {
    };

  grunt.log.write('Running iotivity_nodejs start: ' + iot.start(handlInfo) ).ok();
  });

}
