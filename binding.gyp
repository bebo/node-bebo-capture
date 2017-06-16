{
  "targets": [
    {
      "target_name": "BeboCaptureNative",
      "sources": [ "BeboCaptureNative.cc", "CaptureFunctions.cc", "DesktopListFunctions.cc", "WindowListFunctions.cc", "WinAsyncWorker.cc" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      "link_settings": {
        "libraries": [ "d3d11.lib" ]
      }
    }
  ]
}
