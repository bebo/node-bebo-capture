#include "CaptureFunctions.h"
#include "WinAsyncWorker.h"

#include <string>
#include <cstdint>
#include <cinttypes>
#include <iostream>

using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::Callback;
using v8::Function;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Value;
using v8::Array;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Set;
using Nan::Null;
using Nan::To;

#define EVENT_READ_REGISTRY "Global\\BEBO_CAPTURE_READ_REGISTRY"

class CheckProcessWorker: public WinAsyncWorker {
  protected:
    bool ok = true;
    bool requireAdmin = false;
    std::string message = "";
    DWORD errorCode = 0;
    CaptureEntity *entity;

  public:
    CheckProcessWorker(CaptureEntity *e, Callback *callback)
      : WinAsyncWorker(callback), entity(e){};
    ~CheckProcessWorker() {
    };

    // Executed inside the worker-thread.
    // It is not safe to access V8, or V8 data structures
    // here, so everything we need for input and output
    // should go on `this`.
    void Execute() {
      HWND hwnd = 0;

      if (entity->hwnd != 0) {
        hwnd = reinterpret_cast<HWND>(entity->hwnd);
      } else if (entity->windowClassName.length() != 0 && 
                 entity->windowName.length() != 0) { // window class and window name
        hwnd = FindWindowA(entity->windowClassName.c_str(), entity->windowName.c_str());
      } else if (entity->windowClassName.length() != 0 &&
                 entity->windowName.length() == 0) { // window class only
        hwnd = FindWindowA(entity->windowClassName.c_str(), NULL);
      } else if (entity->windowClassName.length() == 0 &&
                 entity->windowName.length() != 0) { // window name only
        hwnd = FindWindowA(NULL, entity->windowName.c_str());
      }

      if (hwnd == 0) {
        ok = false;
        message = "window not found";
        return;
      }

      DWORD processId = 0;
      GetWindowThreadProcessId(hwnd, &processId);
      if (processId != 0) {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, false, processId);
        ok = true;

        if (processHandle == NULL) {
          errorCode = GetLastError();
          if (errorCode == 5) {
            requireAdmin = true;
          }
        } else {
          requireAdmin = false;
        }

        CloseHandle(processHandle);
      } else {
        ok = false;
        message = "process not found";
      }
    }

    void HandleOKCallback() {
      HandleScope scope;

      Local<Object> obj = Nan::New<Object>();
      Set(obj, New("ok").ToLocalChecked(), New(ok));
      Set(obj, New("error").ToLocalChecked(), New(static_cast<uint32_t>(errorCode)));
      Set(obj, New("require_admin").ToLocalChecked(), New(requireAdmin));
      Set(obj, New("message").ToLocalChecked(), New(message).ToLocalChecked());

      Local<Value> argv[] = {
        Null()
          , obj
      };

      callback->Call(2, argv);
    }

};

class IsRunningAsAdminWorker: public WinAsyncWorker {
  protected:
    bool ok = true;
    bool isElevated = false;

  public:
    IsRunningAsAdminWorker(Callback *callback)
      : WinAsyncWorker(callback){};
    ~IsRunningAsAdminWorker() {
    };

    // Executed inside the worker-thread.
    // It is not safe to access V8, or V8 data structures
    // here, so everything we need for input and output
    // should go on `this`.
    void Execute() {
      HANDLE token = NULL;
      if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(token, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
          isElevated = Elevation.TokenIsElevated;
        }
      }

      if (token) {
        CloseHandle(token);
      }
    }

    void HandleOKCallback() {
      HandleScope scope;

      Local<Object> obj = Nan::New<Object>();
      Set(obj, New("ok").ToLocalChecked(), New(ok));
      Set(obj, New("is_elevated").ToLocalChecked(), New(isElevated));

      Local<Value> argv[] = {
        Null()
          , obj
      };

      callback->Call(2, argv);
    }

};


v8::Local<v8::Value> getValueFromObject(v8::Local<v8::Object> options, std::string key) {
  v8::Local<v8::String> v8str = Nan::New<v8::String>(key).ToLocalChecked();
  return Nan::Get(options, v8str).ToLocalChecked();
}

int getIntFromObject(v8::Local<v8::Object> options, std::string key) {
  return Nan::To<v8::Int32>(getValueFromObject(options, key)).ToLocalChecked()->Value();
}

bool getBoolFromObject(v8::Local<v8::Object> options, std::string key) {
  return Nan::To<v8::Boolean>(getValueFromObject(options, key)).ToLocalChecked()->Value();
}

v8::Local<v8::String> getStringFromObject(v8::Local<v8::Object> options, std::string key) {
  return Nan::To<v8::String>(getValueFromObject(options, key)).ToLocalChecked();
}

double getDoubleFromObject(v8::Local<v8::Object> options, std::string key) {
  return Nan::To<double>(getValueFromObject(options, key)).FromMaybe(0);
}

bool isKeyExists(v8::Local<v8::Object> options, std::string key) {
  v8::Local<v8::String> v8str = Nan::New<v8::String>(key).ToLocalChecked();
  return Nan::Has(options, v8str).FromJust();
}

NAN_METHOD(checkProcessElevation) {
  Local<Object> object = info[0].As<Object>();
  Callback *callback = new Callback(info[1].As<Function>());
  CaptureEntity *entity = new CaptureEntity;

  if (!isKeyExists(object, "windowClassName") && 
      !isKeyExists(object, "windowName") &&
      !isKeyExists(object, "windowHandle")) {
    Local<Value> argv[] = {New("need to have at least one valid: windowClassName, windowName or windowHandle").ToLocalChecked(), Null()};
    callback->Call(2, argv);
    return;
  }

  if (isKeyExists(object, "windowName")) {
    v8::String::Utf8Value value(getStringFromObject(object, "windowName"));
    entity->windowName = *value;
  }

  if (isKeyExists(object, "windowClassName")) {
    v8::String::Utf8Value value(getStringFromObject(object, "windowClassName"));
    entity->windowClassName = *value;
  }

  if (isKeyExists(object, "windowHandle")) {
    v8::String::Utf8Value value(getStringFromObject(object, "windowHandle"));
    std::string::size_type sz = 0;
    try {
      entity->hwnd = std::stoll(*value, &sz, 0); 
    } catch (std::invalid_argument) {
      Local<Value> argv[] = {New("value - invalid argument").ToLocalChecked(), Null()};
      callback->Call(2, argv);
      return;
    } catch (std::out_of_range) {
      Local<Value> argv[] = {New("value - out of range").ToLocalChecked(), Null()};
      callback->Call(2, argv);
      return;
    }
  }

  AsyncQueueWorker(new CheckProcessWorker(entity, callback));
}

NAN_METHOD(isProcessElevated) {
  Callback *callback = new Callback(info[0].As<Function>());
  AsyncQueueWorker(new IsRunningAsAdminWorker(callback));
}

