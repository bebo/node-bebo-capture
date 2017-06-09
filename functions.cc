#include "functions.h"
#include <windows.h>
#include <iostream>

using Nan::Callback;
using v8::Function;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Set;
using Nan::Null;
using Nan::To;

class CaptureEntity {

public:
  char * id = NULL;
  char * label = NULL;
  char * type = NULL;
  char * windowClassName = NULL;
  char * windowName = NULL;
  bool antiCheat = 0;

  ~CaptureEntity() {
	  if (id != NULL) {
		  delete id;
	  }
	  if (label != NULL) {
		  delete label;
	  }
	  if (type != NULL) {
		  delete type;
	  }
	  if (windowName != NULL) {
		  delete windowName;
	  }
	  if (windowClassName != NULL) {
		  delete windowClassName;
	  }
  }
};

HRESULT RegGetBeboSZ(char * szValueName, char * data, LPDWORD datasize) {

	WCHAR valueName[1024] = { 0 };
	BYTE bytes[1024];
	HKEY hKey;
	LSTATUS lstatus;
	if (!MultiByteToWideChar(CP_UTF8, 0, szValueName, strlen(szValueName), valueName, 1024)) {
	  return E_INVALIDARG;
	}

	if (data == NULL) {
	  return E_INVALIDARG;
	}

	lstatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Bebo\\GameCapture", 0, KEY_READ, &hKey);

	if (lstatus != ERROR_SUCCESS) {
        std::cout << "can't open key: " << szValueName << std::endl;
		return E_INVALIDARG;
	}
    std::cout << "opened key - now looking for " << szValueName << std::endl;

	DWORD dwType = REG_SZ;

	// Check input parameters...

	WCHAR wdata[1024];
	// Get dword value from the registry...
	DWORD wSize = 1024;
	lstatus = RegQueryValueExW(hKey, valueName, 0, &dwType, (LPBYTE) wdata, &wSize);

	RegCloseKey(hKey);

	if (lstatus != ERROR_SUCCESS) {
        std::cout << "error reading key " << lstatus << std::endl;
		return HRESULT_FROM_WIN32(lstatus);
	} else if (dwType != REG_SZ) {
        std::cout << "wrong type " << std::endl;
		return DISP_E_TYPEMISMATCH;
	}

	*datasize = WideCharToMultiByte(CP_UTF8,0, wdata, wSize, data, *datasize, NULL, NULL);

	return NOERROR;
}


char * getSZ(char * key) {
  DWORD size = 1024;
  char * value = (char *)malloc(size);
  HRESULT status = RegGetBeboSZ(key, value, &size);
  if (status == NOERROR) {
    return value;
  } else {
	free(value);
	return NULL;
  }
}

class GetCaptureWorker: public AsyncWorker {
public:
	GetCaptureWorker(Callback *callback)
		: AsyncWorker(callback) {};
	~GetCaptureWorker() {
		if (capture) {
			delete capture;
		}
	};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute() {

		capture = new CaptureEntity();
		capture->id = getSZ("CaptureId");
		capture->type = getSZ("CaptureType");
		capture->label = getSZ("CaptureLabel");
		capture->windowClassName = getSZ("CaptureWindowClassName");
		capture->windowName = getSZ("CaptureWindowNameName");
		//capture->label = getSZ("CaptureAntiCheat"); // TODO boolean

	}

	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback() {
		HandleScope scope;

		Local<Object> obj = Nan::New<Object>();
		if (capture->id) {
          Set(obj, New("id").ToLocalChecked(), New(capture->id).ToLocalChecked());
		}
		if (capture->label) {
  		  Set(obj, New("label").ToLocalChecked(),New(capture->label).ToLocalChecked());
		}
		if (capture->type) {
			Set(obj, New("type").ToLocalChecked(), New(capture->type).ToLocalChecked());
		}
		if (capture->windowName) {
			Set(obj, New("windowName").ToLocalChecked(), New(capture->windowName).ToLocalChecked());
		}
		if (capture->windowClassName) {
			Set(obj, New("windowClassName").ToLocalChecked(), New(capture->windowClassName).ToLocalChecked());
		}
		//TODO boolean antiCheat

		Local<Value> argv[] = {
			Null()
			, obj
		};

		callback->Call(2, argv);
	}

private:
	CaptureEntity *capture;
};

NAN_METHOD(getCapture) {

	Callback *callback = new Callback(info[0].As<Function>());
	AsyncQueueWorker(new GetCaptureWorker(callback));

}