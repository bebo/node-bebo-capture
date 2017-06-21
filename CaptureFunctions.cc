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


class GetCaptureWorker: public WinAsyncWorker {

public:
	GetCaptureWorker(Callback *callback)
		: WinAsyncWorker(callback) {};
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
		Set(obj, New("label").ToLocalChecked(), New(capture->label).ToLocalChecked());
		Set(obj, New("type").ToLocalChecked(), New(capture->type).ToLocalChecked());
		Set(obj, New("windowName").ToLocalChecked(), New(capture->windowName).ToLocalChecked());
		Set(obj, New("windowClassName").ToLocalChecked(), New(capture->windowClassName).ToLocalChecked());
		Set(obj, New("antiCheat").ToLocalChecked(), New(capture->antiCheat));

        if (capture->type == "gdi") {
            Set(obj, New("once").ToLocalChecked(), New(capture->once));
            if (capture->hwnd != 0) {
                char windowHandle[20] = {0};
                std::sprintf(windowHandle, "0x%016" PRIx64, capture->hwnd);
                Set(obj, New("windowHandle").ToLocalChecked(), New(windowHandle).ToLocalChecked());
            }
		    Set(obj, New("exeFullName").ToLocalChecked(), New(capture->exeFullName).ToLocalChecked());
        }
                

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

	bool chkGetQWord(HKEY hkey, char * key, uint64_t *data) {
		std::string msg("Can't read from registry (");
		msg.append(key);
		msg.append(") ");
		return chk(getQWord(hkey, key, data), msg);
	}

	bool chkPutQWord(HKEY hkey, char * key, uint64_t data) {
		std::string msg("Can't write to registry (");
		msg.append(key);
		msg.append(") ");
		return chk(putQWord(hkey, key, data), msg);
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
		if (!chkGetSZ(hkey, "CaptureWindowName", capture->windowName)) return;
		if (!chkGetSZ(hkey, "CaptureWindowClassName", capture->windowClassName)) return;
		if (!chkGetSZ(hkey, "CaptureExeFullName", capture->exeFullName)) return;
        if (!chkGetQWord(hkey, "CaptureWindowHandle", &capture->hwnd)) return;
        if (!chkGetBool(hkey, "CaptureAntiCheat", &capture->antiCheat)) return;
        if (!chkGetBool(hkey, "CaptureOnce", &capture->once)) return;

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

		if (!chkPutSZ(hkey, "CaptureId", capture->id)) return;
		if (!chkPutSZ(hkey, "CaptureType", capture->type)) return;
		if (!chkPutSZ(hkey, "CaptureLabel", capture->label)) return;
		if (!chkPutSZ(hkey, "CaptureWindowName", capture->windowName)) return;
		if (!chkPutSZ(hkey, "CaptureWindowClassName", capture->windowClassName)) return;
		if (!chkPutSZ(hkey, "CaptureExeFullName", capture->exeFullName)) return;
        if (!chkPutQWord(hkey, "CaptureWindowHandle", capture->hwnd)) return;
		if (!chkPutBool(hkey, "CaptureAntiCheat", capture->antiCheat)) return;
		if (!chkPutBool(hkey, "CaptureOnce", capture->once)) return;
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


    if (!info[5].IsEmpty()) {
        Nan::Utf8String nan_string(info[5]);
        std::string hwnd(*nan_string);
        if (hwnd.size() > 0) {
            try {
                capture->hwnd = stoull(hwnd, 0, 16);
            } catch (std::invalid_argument) {
                // TODO: throw back js exception...
                std::cout << "Invalid number string" << hwnd << std::endl;
            }
        }
	}

	capture->exeFullName = cpStringArg(info, 6);
    std::cout << "exeFullName" << capture->exeFullName << std::endl;

	Nan::Maybe<bool> antiCheat = Nan::To<bool>(info[7]);
	if (!info[7].IsEmpty()) {
		capture->antiCheat = antiCheat.ToChecked();
	}
	Nan::Maybe<bool> once = Nan::To<bool>(info[8]);
	if (!info[8].IsEmpty()) {
		capture->once = once.ToChecked();
	}

	Callback *callback = new Callback(info[9].As<Function>());
	AsyncQueueWorker(new SetCaptureWorker(capture, callback));
}
