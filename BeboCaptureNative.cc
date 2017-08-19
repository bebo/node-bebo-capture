#include "CaptureFunctions.h"
#include "ConstraintFunctions.h"
#include "DesktopListFunctions.h"
#include "WindowListFunctions.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll)
{
  Nan::Set(target, Nan::New("getCapture").ToLocalChecked(),
           Nan::GetFunction(Nan::New<FunctionTemplate>(getCapture)).ToLocalChecked());
  Nan::Set(target, Nan::New("setCapture").ToLocalChecked(),
           Nan::GetFunction(Nan::New<FunctionTemplate>(setCapture)).ToLocalChecked());
  Nan::Set(target, Nan::New("getConstraint").ToLocalChecked(),
           Nan::GetFunction(Nan::New<FunctionTemplate>(getConstraint)).ToLocalChecked());
  Nan::Set(target, Nan::New("setConstraint").ToLocalChecked(),
           Nan::GetFunction(Nan::New<FunctionTemplate>(setConstraint)).ToLocalChecked());
  Nan::Set(target, Nan::New("getDesktops").ToLocalChecked(),
           Nan::GetFunction(Nan::New<FunctionTemplate>(getDesktops)).ToLocalChecked());
  Nan::Set(target, Nan::New("getWindowList").ToLocalChecked(),
           Nan::GetFunction(Nan::New<FunctionTemplate>(getWindowList)).ToLocalChecked());
}

NODE_MODULE(NativeExtension, InitAll)
