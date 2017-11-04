{
  "targets": [
    {
      "target_name": "BeboCaptureNative",
      "sources": [ "BeboCaptureNative.cc", "CaptureFunctions.cc", "ConstraintFunctions.cc", "DesktopListFunctions.cc", "WindowListFunctions.cc", "ProcessFunctions.cc", "WinAsyncWorker.cc" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      "defines": [
            "_HAS_EXCEPTIONS=1",
            "_UNICODE=1",
            "UNICODE=1"
      ],
      "link_settings": {
        "libraries": [ "d3d11.lib", "Psapi.lib" ]
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      },
    }
  ],
}
