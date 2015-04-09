{
  "variables": {
    "iotivity_sdk": "<!(echo $HOME)/git/iotivity"
  },
  "targets": [
    {
      "target_name": "iotivity_nodejs",
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<@(iotivity_sdk)/resource/csdk/logger/include",
        "<@(iotivity_sdk)/resource/csdk/stack/include",
        "<@(iotivity_sdk)/resource/csdk/ocsocket/include",
        "<@(iotivity_sdk)/resource/oc_logger/include",
        "<@(iotivity_sdk)/extlibs/cjson/",
      ],
      "sources": [
        "iotivity_nodejs.cpp",
        "csdkWrapper.cpp"
      ],
      "cflags_cc": [ "-std=c++11" ],
      "libraries":[
        "-L<@(iotivity_sdk)/resource/csdk/linux/release",
        "-loctbstack"
      ]
    }
  ]
}
