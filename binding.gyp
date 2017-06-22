{
  "targets": [
    {
      "target_name": "BeboCaptureNative",
      "sources": [ "BeboCaptureNative.cc", "CaptureFunctions.cc", "DesktopListFunctions.cc", "WindowListFunctions.cc", "WinAsyncWorker.cc" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      "defines": [
            "_HAS_EXCEPTIONS=1"
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
