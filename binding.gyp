{
  "targets": [
    {
      "target_name": "BeboCaptureNative",
      "sources": [ "BeboCaptureNative.cc", "CaptureFunctions.cc", "DesktopListFunctions.cc", "WindowListFunctions.cc", "WinAsyncWorker.cc" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      "link_settings": {
        "libraries": [ "d3d11.lib" ]
      },
      "target_defaults": {
        "configurations": {
          "Debug": {
            "defines": [ "DEBUG", "_DEBUG" ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "RuntimeLibrary": 3,
                "ExceptionHandling": 1,
                "AdditionalOptions": [ "/EHsc" ]
              }
            }
          },
          "Release": {
            "VCCLCompilerTool": {
              "RuntimeLibrary": 2,
              "ExceptionHandling": 1,
              "AdditionalOptions": [ "/EHsc" ]
            }
          }
        }
      }
    }
  ]
}
