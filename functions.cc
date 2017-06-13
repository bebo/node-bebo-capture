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

	CaptureEntity() : id(NULL), label(NULL), type(NULL), windowClassName(NULL), windowName(NULL), antiCheat(0) {};

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


HRESULT RegOpen(REGSAM samDesired, HKEY * hkey) {
	LSTATUS lstatus = 0;
	lstatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Bebo\\GameCapture", 0, samDesired, hkey);

	if (lstatus != ERROR_SUCCESS) {
		std::cout << "can't open registry: " << lstatus << std::endl;
		return E_INVALIDARG;
	}

	std::cout << "opened registry successfully: " << lstatus << std::endl;
	return NOERROR;
}

HRESULT RegClose(HKEY hkey) {
	LSTATUS lstatus = RegCloseKey(hkey);
	if (lstatus != ERROR_SUCCESS) {
		std::cout << "can't close registry: " << lstatus << std::endl;
		return E_INVALIDARG;
	}
	return NOERROR;
}

HRESULT RegGetBeboSZ(HKEY hkey, char * szValueName, char * data, LPDWORD datasize) {

	LSTATUS lstatus = 0;
	WCHAR valueName[1024] = { 0 };
	if (!MultiByteToWideChar(CP_UTF8, 0, szValueName, strlen(szValueName), valueName, 1024)) {
		return E_INVALIDARG;
	}

	if (data == NULL) {
		return E_INVALIDARG;
	}

	std::cout << "looking for " << szValueName << std::endl;

	DWORD dwType = REG_SZ;

	// Check input parameters...

	WCHAR wdata[1024] = { 0 };
	// Get dword value from the registry...
	DWORD wSize = 1024;
	lstatus = RegQueryValueExW(hkey, valueName, 0, &dwType, (LPBYTE)wdata, &wSize);

	if (lstatus != ERROR_SUCCESS) {
		std::cout << "error reading key " << szValueName << ":" << lstatus << std::endl;
		return HRESULT_FROM_WIN32(lstatus);
	}
	else if (dwType != REG_SZ) {
		std::cout << "wrong type " << std::endl;
		return DISP_E_TYPEMISMATCH;
	}

	*datasize = WideCharToMultiByte(CP_UTF8, 0, wdata, wSize, data, *datasize, NULL, NULL);

	return NOERROR;
}


char * getSZ(HKEY hkey, char * key) {
	DWORD size = 1024;
	char * value = (char *)malloc(size);
	HRESULT status = RegGetBeboSZ(hkey, key, value, &size);
	if (status == NOERROR) {
		return value;
	}
	else {
		free(value);
		return NULL;
	}
}

HRESULT RegGetDWord(HKEY hKey, char * szValueName, DWORD * lpdwResult) {

	// Given a value name and an hKey returns a DWORD from the registry.
	// eg. RegGetDWord(hKey, TEXT("my dword"), &dwMyValue);

	LONG lResult;
	DWORD dwDataSize = sizeof(DWORD);
	DWORD dwType = REG_DWORD;
	WCHAR valueName[1024] = { 0 };

	if (!MultiByteToWideChar(CP_UTF8, 0, szValueName, strlen(szValueName), valueName, 1024)) {
		return E_INVALIDARG;
	}

	// Check input parameters...
	if (hKey == NULL || lpdwResult == NULL) return E_INVALIDARG;

	// Get dword value from the registry...
	lResult = RegQueryValueExW(hKey, valueName, 0, &dwType, (LPBYTE) lpdwResult, &dwDataSize );

	// Check result and make sure the registry value is a DWORD(REG_DWORD)...
	if (lResult != ERROR_SUCCESS) return HRESULT_FROM_WIN32(lResult);
	else if (dwType != REG_DWORD) return DISP_E_TYPEMISMATCH;

	return NOERROR;
}

HRESULT getBool(HKEY hkey, char * key, bool * val) {
	DWORD value = 0;
	HRESULT status = RegGetDWord(hkey, key, &value);
	if (status == NOERROR) {
		*val = value == 1;
	}
	return status;
}

HRESULT putSZ(HKEY hkey, char * key, char * value) {
	if (value == NULL) {
		value = "";
    }
    std::cout << "writing - key: " << key << " value: " << value << std::endl;
	WCHAR valueName[1024] = { 0 };
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024)) {
	  return E_INVALIDARG;
	}

	if (value == NULL) {
	  return E_INVALIDARG;
	}
	WCHAR wdata[2048] = { 0 };
	DWORD wSize = MultiByteToWideChar(CP_UTF8, 0, value, strlen(value), wdata, 2048);
	if ((wSize == 0 && strlen(value) != 0) || wSize == 0x0000FFFD0) {
	  std::cout << "nah" << std::endl;
	  return E_INVALIDARG;
	}
	wSize = wSize * 2 + 2; // write two \0

    std::cout << "now looking for " << key << std::endl;

	DWORD dwType = REG_SZ;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE) wdata, wSize);

	if (lstatus != ERROR_SUCCESS) {
        std::cout << "error writing key " << lstatus << std::endl;
		return HRESULT_FROM_WIN32(lstatus);
	} 

	return NOERROR;
}

HRESULT putBool(HKEY hkey, char * key, bool val) {
	WCHAR valueName[1024] = { 0 };
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024)) {
	  return E_INVALIDARG;
	}

	DWORD value = val;

	DWORD dwType = REG_DWORD;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE) &value, sizeof(value));

	if (lstatus != ERROR_SUCCESS) {
        std::cout << "error writing key " << lstatus << std::endl;
		return HRESULT_FROM_WIN32(lstatus);
	} 

	return NOERROR;
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
		HKEY hkey = NULL;
		HRESULT hresult = RegOpen(KEY_READ, &hkey);
		this->readData(hkey);
		RegClose(hkey);
	}

	void readData(HKEY hkey) {
		std::cout << "READ HERE" << std::endl;
		capture = new CaptureEntity();
		capture->id = getSZ(hkey, "CaptureId");
		capture->type = getSZ(hkey, "CaptureType");
		if (capture->type == NULL) {
			capture->type = strdup("inject");
		}
		capture->label = getSZ(hkey, "CaptureLabel");
		capture->windowClassName = getSZ(hkey, "CaptureWindowClassName");
		capture->windowName = getSZ(hkey, "CaptureWindowName");
        getBool(hkey, "CaptureAntiCheat", &capture->antiCheat);
		//capture->label = getSZ("CaptureAntiCheat"); // TODO boolean
	}

	void HandleErrorCallback() {
		printf("error occured\n");
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

		Set(obj, New("antiCheat").ToLocalChecked(), New(capture->antiCheat));

		Local<Value> argv[] = {
			Null()
			, obj
		};

		callback->Call(2, argv);
	}

protected:
	CaptureEntity *capture;
};

class SetCaptureWorker: public GetCaptureWorker {
public:
	SetCaptureWorker(CaptureEntity *capture, Callback *callback)
		: GetCaptureWorker(callback)  {
		this->capture = capture;
	};
	~SetCaptureWorker() {
	};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute() {

		std::cout << "WRITE HERE" << std::endl;
		HKEY hkey = NULL;
		HRESULT hresult = RegOpen(KEY_ALL_ACCESS, &hkey);
		// TODO handle error cases...
		putSZ(hkey, "CaptureType", capture->type);
		putSZ(hkey, "CaptureId", capture->id);
		putSZ(hkey, "CaptureLabel", capture->label);
		putSZ(hkey, "CaptureWindowName", capture->windowName);
		putSZ(hkey, "CaptureWindowClassName", capture->windowClassName);
		putBool(hkey, "CaptureAntiCheat", capture->antiCheat);
		delete(capture);
		readData(hkey);
		RegClose(hkey);
	}


};

NAN_METHOD(getCapture) {
	Callback *callback = new Callback(info[0].As<Function>());
	AsyncQueueWorker(new GetCaptureWorker(callback));

}

char * cpStringArg(Nan::NAN_METHOD_ARGS_TYPE arg, int i) {

	if (!arg[i].IsEmpty()) {
		Nan::Utf8String nan_string(arg[i]);
		std::string name(*nan_string);
		return strdup(name.c_str());
	}
	return strdup("");
}

NAN_METHOD(setCapture) {

	auto capture = new CaptureEntity();
	capture->type = cpStringArg(info, 0);
	capture->id = cpStringArg(info, 1);
	capture->label = cpStringArg(info, 2);
	capture->windowName = cpStringArg(info, 3);
	capture->windowClassName = cpStringArg(info, 4);
	Nan::Maybe<bool> antiCheat = Nan::To<bool>(info[5]);
	if (!info[5].IsEmpty()) {
		capture->antiCheat = antiCheat.ToChecked();
	}

	Callback *callback = new Callback(info[6].As<Function>());
	AsyncQueueWorker(new SetCaptureWorker(capture, callback));
}