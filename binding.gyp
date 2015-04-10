{
  "variables": {
    "iotivity_sdk": "<!(echo $IOTIVITY_HOME)"
  },
  "targets": [
    {
      "target_name": "iotivity_wrapper_native",
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<@(iotivity_sdk)/resource/csdk/logger/include",
        "<@(iotivity_sdk)/resource/csdk/stack/include",
        "<@(iotivity_sdk)/resource/csdk/ocsocket/include",
        "<@(iotivity_sdk)/resource/oc_logger/include",
        "<@(iotivity_sdk)/extlibs/cjson/",
      ],
      "sources": [
        "iotivity_wrapper_native.cpp",
        "csdkWrapper.cpp"
      ],
      "cflags_cc": [ "-std=c++0x" ],
      "libraries":[
        "-L<@(iotivity_sdk)/resource/csdk/linux/release",
        "-loctbstack"
      ]
    }
  ]
}
