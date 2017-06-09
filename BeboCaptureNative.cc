#include "functions.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("getCapture").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(getCapture)).ToLocalChecked());
}

NODE_MODULE(NativeExtension, InitAll)
