{
  "targets":[{
    "target_name": "node_wifi_info",
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "sources": [
        "cppsrc/main.cpp"
      ],
      "libraries": [],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      "conditions": [
        ['OS=="win"', {
          "sources": [
            "cppsrc/WindowsWifiInfo.cpp"
          ]
        }],
        ['OS=="linux"', {
          "sources": [
            "cppsrc/StubWifiInfo.cpp"
          ]
        }],
        ['OS=="mac"', {
          "sources": [
            "cppsrc/WifiInfo.mm"
          ],
          'link_settings': {
            'libraries': [
              'CoreWLAN.framework',
              'Foundation.framework',
            ]
          },
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'OTHER_CFLAGS': [
              '-ObjC++',
              '-std=c++11'
            ]
          } 
        }]
      ]
  }]
}
