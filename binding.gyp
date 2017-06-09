{
    "targets": [
        {
            "target_name": "BeboCaptureNative",
            "sources": [ "BeboCaptureNative.cc", "functions.cc" ],
            "include_dirs" : [ "<!(node -e \"require('nan')\")" ]
        }
    ],
}
