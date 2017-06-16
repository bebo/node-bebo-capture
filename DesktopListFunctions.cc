#include "WinAsyncWorker.h"
#include "DesktopListFunctions.h"

#include <windows.h>
#include <iostream>
#include <string.h>
#include <DXGI.h>
#include <D3DCommon.h>
#include <D3D11.h>
#include <list>
#include "WinAsyncWorker.h"

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

class GetDesktopsWorker: public AsyncWorker {

public:
	GetDesktopsWorker(Callback *callback)
		: AsyncWorker(callback) {
		};
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
						std::string id("desktop:");
						id.append(std::to_string(i));
						id.append(":");
						id.append(std::to_string(j));
						screens.push_back(id);
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

		Local<Array> result = Nan::New<Array>(screens.size());
		int screenNr = 1;
		for (std::list<std::string>::iterator it = screens.begin(); it != screens.end(); ++it) {
		  Local<Object> obj = Nan::New<Object>();
		  std::string id = *it;
		  std::string label("Screen ");
		  label.append(std::to_string(screenNr));

          Set(obj, New("id").ToLocalChecked(), New(id).ToLocalChecked());
  		  Set(obj, New("label").ToLocalChecked(),New(label).ToLocalChecked());
		  Set(obj, New("type").ToLocalChecked(), New("desktop").ToLocalChecked());
		  Nan::Set(result, screenNr-1, obj);
		  screenNr++;
		}

		Local<Value> argv[] = { Null() , result };

		callback->Call(2, argv);
	}

protected:
	std::list<std::string> screens;


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
