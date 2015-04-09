iot = require("bindings")("iotivity_nodejs");

module.exports = function(grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
  });

  grunt.registerTask('version', 'Check IoTivity', function() {
    grunt.log.write('Running iotivity_nodejs version: ' + iot.version() ).ok();
  });
}