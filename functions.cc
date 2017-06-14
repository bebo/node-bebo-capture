#include "functions.h"
#include <windows.h>
#include <iostream>
#include <string.h>
#include <DXGI.h>
#include <D3DCommon.h>
#include <D3D11.h>

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

class CaptureEntity {

public:
	std::string id;
	std::string label;
    std::string type;
	std::string windowClassName ;
	std::string windowName;
	bool antiCheat = 0;

	CaptureEntity() {};

	~CaptureEntity() {
	}
};


HRESULT RegOpen(REGSAM samDesired, HKEY * hkey) {
	LSTATUS lstatus = 0;
	lstatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Bebo\\GameCapture", 0, samDesired, hkey);

	if (lstatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lstatus);
	}

	return NOERROR;
}

HRESULT RegClose(HKEY hkey) {
	LSTATUS lstatus = RegCloseKey(hkey);
	if (lstatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lstatus);
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

	DWORD dwType = REG_SZ;

	// Check input parameters...

	WCHAR wdata[1024] = { 0 };
	// Get dword value from the registry...
	DWORD wSize = 1024;
	lstatus = RegQueryValueExW(hkey, valueName, 0, &dwType, (LPBYTE)wdata, &wSize);

	if (lstatus != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lstatus);
	}
	else if (dwType != REG_SZ) {
		return DISP_E_TYPEMISMATCH;
	}

	*datasize = WideCharToMultiByte(CP_UTF8, 0, wdata, wSize, data, *datasize, NULL, NULL);

	return NOERROR;
}


HRESULT getSZ(HKEY hkey, char * key, std::string & value) {
	DWORD size = 1024;
	char val[1024] = { 0 };
	HRESULT status = RegGetBeboSZ(hkey, key, val, &size);
	if (status == NOERROR && strlen(val) < 1024) {
		value.assign(val);
	}
	return status;
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
	if (lResult != ERROR_SUCCESS) {
		return HRESULT_FROM_WIN32(lResult);
	} else if (dwType != REG_DWORD) {
		return DISP_E_TYPEMISMATCH;
	}

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

HRESULT putSZ(HKEY hkey, char * key, std::string & value) {

	WCHAR valueName[1024] = { 0 };
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024)) {
	  return E_INVALIDARG;
	}

	WCHAR wdata[2048] = { 0 };
	DWORD wSize = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), strlen(value.c_str()), wdata, 2048);
	if ((wSize == 0 && strlen(value.c_str()) != 0) || wSize == 0x0000FFFD0) {
	  return E_INVALIDARG;
	}
	wSize = wSize * 2 + 2; // write two \0


	DWORD dwType = REG_SZ;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE) wdata, wSize);

	if (lstatus != ERROR_SUCCESS) {
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
		return HRESULT_FROM_WIN32(lstatus);
	} 

	return NOERROR;
}

const char * errno_to_text(HRESULT errorNumber) {
	const char * errorMessage;
	DWORD nLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errorNumber,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errorMessage,
		0,
		NULL);
	return errorMessage;
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
		if (!chk(RegOpen(KEY_READ, &hkey), "Can't open registry")) {
			return;
		}
		this->readData(hkey);
		chk(RegClose(hkey), "Can't close registry");
	}


	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback() {
		HandleScope scope;

		Local<Object> obj = Nan::New<Object>();
        Set(obj, New("id").ToLocalChecked(), New(capture->id).ToLocalChecked());
  		Set(obj, New("label").ToLocalChecked(),New(capture->label).ToLocalChecked());
		Set(obj, New("type").ToLocalChecked(), New("desktop").ToLocalChecked());

		Local<Value> argv[] = {
			Null()
			, obj
		};

		callback->Call(2, argv);
	}

protected:
	CaptureEntity *capture;

	bool chk(HRESULT hresult, std::string & msg) {
		std::string mymsg(msg);
		if (hresult != NOERROR) { 
			mymsg.append(errno_to_text(hresult));
			SetErrorMessage(mymsg.c_str());
			return false;
		}
		return true;
	}

	bool chk(HRESULT hresult, const char * msg) {
		return chk(hresult, std::string(msg));
	}

	bool chkGetBool(HKEY hkey, char * key, bool *data) {
		std::string msg("Can't read from registry (");
		msg.append(key);
		msg.append(") ");
		return chk(getBool(hkey, key, data), msg);
	}

	bool chkPutBool(HKEY hkey, char * key, bool data) {
		std::string msg("Can't write to registry (");
		msg.append(key);
		msg.append(") ");
		return chk(putBool(hkey, key, data), msg);
	}

	bool chkPutSZ(HKEY hkey, char * key, std::string & data) {
		std::string msg("Can't write to registry (");
		msg.append(key);
		msg.append(") ");
		return chk(putSZ(hkey, key, data), msg);
	}

    bool chkGetSZ(HKEY hkey, char * key, std::string & value) {
		std::string msg("Can't read from registry (");
		msg.append(key);
		msg.append(") ");
		return chk(getSZ(hkey, key, value), msg);
	}

	void readData(HKEY hkey) {

		capture = new CaptureEntity();
		if (!chkGetSZ(hkey, "CaptureId", capture->id)) return;
		if (!chkGetSZ(hkey, "CaptureType", capture->type)) return;
		if (capture->type.size() == 0) {
			capture->type.assign("inject");
		}
		if (!chkGetSZ(hkey, "CaptureLabel", capture->label)) return;
		if (!chkGetSZ(hkey, "CaptureWindowClassName", capture->windowClassName)) return;
		if (!chkGetSZ(hkey, "CaptureWindowName", capture->windowName)) return;
        if (!chkGetBool(hkey, "CaptureAntiCheat", &capture->antiCheat)) return;
	}
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

		HKEY hkey = NULL;
		if (!chk(RegOpen(KEY_ALL_ACCESS, &hkey), "Can't open registry for write")) {
			return;
		}

		if (!chkPutSZ(hkey, "CaptureType", capture->type)) return;
		if (!chkPutSZ(hkey, "CaptureId", capture->id)) return;
		if (!chkPutSZ(hkey, "CaptureLabel", capture->label)) return;
		if (!chkPutSZ(hkey, "CaptureWindowName", capture->windowName)) return;
		if (!chkPutBool(hkey, "CaptureAntiCheat", capture->antiCheat)) return;
		delete(capture);
		readData(hkey);
		chk(RegClose(hkey), "Can't close registry");
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

class GetDesktopsWorker: public AsyncWorker {

public:
	GetDesktopsWorker(Callback *callback)
		: AsyncWorker(callback), screenCount(0) {};
	~GetDesktopsWorker() {
	};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute() {
		ID3D11Device *device;

		D3D_DRIVER_TYPE DriverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT NumDriverTypes = ARRAYSIZE(DriverTypes);


		HRESULT hr;
		// Create device
		for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
		{
			hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, NULL, 0,
				D3D11_SDK_VERSION, &device, NULL, NULL);
			if (SUCCEEDED(hr))
			{
				// Device creation success, no need to loop anymore
				break;
			}
		}
		if (!chk(hr, "can't open d3d device")) return;

		IDXGIDevice* dxgiDevice = nullptr;
		hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (!chk(hr, "can't open dxgi device")) return;

		IDXGIAdapter* dxgiAdapter = nullptr;
		hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
		if (!chk(hr, "can't open dxgi adapter")) return;

		IDXGIFactory * dxgiFactory = nullptr;
		hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
		if (!chk(hr, "can't open dxgi factory")) return;

		UINT i = 0;
		IDXGIAdapter * a;
		while (dxgiFactory->EnumAdapters(i, &a) != DXGI_ERROR_NOT_FOUND) {
			UINT j = 0;
			IDXGIOutput * pOutput;
			while (a->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_OUTPUT_DESC desc = { 0 };
				HRESULT hr = pOutput->GetDesc(&desc);
				if (SUCCEEDED(hr)) {
					if (desc.AttachedToDesktop) {
						// MONITORINFOEX mi;
						// ZeroMemory(&mi, sizeof(mi));
						// mi.cbSize = sizeof(mi);
						// GetMonitorInfoA(desc.Monitor, &mi);
						// std::cout << mi.szDevice << " " << desc.Monitor << std::endl;

						screenCount++;
					} 
				} 
				pOutput->Release();
				++j;
			}
			a->Release();
			++i;
		}
		chk(hr, "could not find monitor");

	}


	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback() {
		HandleScope scope;

		Local<Array> result = Nan::New<Array>(screenCount);
		for (int i = 0; i < screenCount; i++) {
		  Local<Object> obj = Nan::New<Object>();
		  std::string id("desktop:");
		  char nr[4] = { 0 };
		  sprintf(nr, "%d", i);
		  id.append(nr);
		  
		  std::string label("Screen ");
		  sprintf(nr, "%d", i+1);
		  label.append(nr);

          Set(obj, New("id").ToLocalChecked(), New(id).ToLocalChecked());
  		  Set(obj, New("label").ToLocalChecked(),New(label).ToLocalChecked());
		  Set(obj, New("type").ToLocalChecked(), New("desktop").ToLocalChecked());
		  Nan::Set(result, i, obj);
		}

		Local<Value> argv[] = { Null() , result };

		callback->Call(2, argv);
	}

protected:
	int screenCount;


	bool chk(HRESULT hresult, std::string & msg) {
		std::string mymsg(msg);
		if (hresult != NOERROR) { 
			mymsg.append(errno_to_text(hresult));
			SetErrorMessage(mymsg.c_str());
			return false;
		}
		return true;
	}

	bool chk(HRESULT hresult, const char * msg) {
		return chk(hresult, std::string(msg));
	}

};

NAN_METHOD(getDesktops) {
	Callback *callback = new Callback(info[0].As<Function>());
	AsyncQueueWorker(new GetDesktopsWorker(callback));
}