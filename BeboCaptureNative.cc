#include "CaptureFunctions.h"
#include "DesktopListFunctions.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("getCapture").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(getCapture)).ToLocalChecked());
  Nan::Set(target, Nan::New("setCapture").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(setCapture)).ToLocalChecked());
  Nan::Set(target, Nan::New("getDesktops").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(getDesktops)).ToLocalChecked());
}

NODE_MODULE(NativeExtension, InitAll)
