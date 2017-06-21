#include "WinAsyncWorker.h"
#include "DesktopListFunctions.h"

#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <string.h>
#include <DXGI.h>
#include <D3DCommon.h>
#include <D3D11.h>
#include <list>
#include "WinAsyncWorker.h"
#include <functional>
#include <string>
#include <cstdint>
#include <cinttypes>


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

// http://stackoverflow.com/questions/7277366/why-does-enumwindows-return-more-windows-than-i-expected
BOOL IsAltTabWindow(HWND hwnd)
{
	TITLEBARINFO ti;
	HWND hwndTry, hwndWalk = NULL;

	if (!IsWindowVisible(hwnd))
		return FALSE;
	hwndTry = GetAncestor(hwnd, GA_ROOTOWNER);
	while (hwndTry != hwndWalk)
	{
		hwndWalk = hwndTry;
		hwndTry = GetLastActivePopup(hwndWalk);
		if (IsWindowVisible(hwndTry))
			break;
	}
	if (hwndWalk != hwnd)
		return FALSE;

	// the following removes some task tray programs and "Program Manager"
	ti.cbSize = sizeof(ti);
	GetTitleBarInfo(hwnd, &ti);
	if (ti.rgstate[0] & STATE_SYSTEM_INVISIBLE && !(ti.rgstate[0] & STATE_SYSTEM_FOCUSABLE))
		return FALSE;

	// Tool windows should not be displayed either, these do not appear in the
	// task bar.
	if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
		return FALSE;

	return TRUE;
}


BOOL CALLBACK EnumerationCallbackWrapper(HWND hWnd, LPARAM lParam);

class GetWindowListWorker: public WinAsyncWorker {

public:
	GetWindowListWorker(Callback *callback)
		: WinAsyncWorker(callback) {
		};
	~GetWindowListWorker() {
	};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute() {
		EnumWindows(EnumerationCallbackWrapper, (LPARAM) this);
	}

	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback() {
		HandleScope scope;

		int i = 0;
		Local<Array> result = Nan::New<Array>(windowList.size());
		for (std::list<CaptureEntity *>::iterator it = windowList.begin(); it != windowList.end(); ++it) {
			CaptureEntity *capture = *it;
			Local<Object> obj = Nan::New<Object>();
			Set(obj, New("windowName").ToLocalChecked(), New(capture->windowName).ToLocalChecked());
			Set(obj, New("windowClassName").ToLocalChecked(), New(capture->windowClassName).ToLocalChecked());
			char windowHandle[20] = {0};
			std::sprintf(windowHandle, "0x%016" PRIx64, capture->hwnd);
			Set(obj, New("windowHandle").ToLocalChecked(), New(windowHandle).ToLocalChecked());
			Set(obj, New("exeFullName").ToLocalChecked(), New(capture->exeFullName).ToLocalChecked());
		    Nan::Set(result, i, obj);
			i++;
		}

		Local<Value> argv[] = {Null() ,result};

		callback->Call(2, argv);
	}

	BOOL EnumerationCallback(HWND hWnd) {
		WCHAR title[1024] = { 0 };
		WCHAR class_name[1024] = { 0 };
		DWORD pid = 0;

		if (!IsAltTabWindow(hWnd)) {
			return TRUE;
		}
        CaptureEntity *capture = new CaptureEntity();

		GetWindowTextW(hWnd, title, 1024);
		GetClassNameW(hWnd, class_name, 1024);
		GetWindowThreadProcessId(hWnd, &pid);

		char title_utf8[1024] = { 0 };
		int title_utf8_size = 1024;
        WideCharToMultiByte(CP_UTF8, 0, title, sizeof(title), title_utf8, title_utf8_size, NULL, NULL);
		capture->windowName.append(title_utf8);

		char class_name_utf8[1024] = { 0 };
		int class_name_utf8_size = 1024;
                WideCharToMultiByte(CP_UTF8, 0, class_name, sizeof(class_name), class_name_utf8, class_name_utf8_size, NULL, NULL);
		capture->windowClassName.append(class_name_utf8);
                capture->hwnd = (uint64_t) hWnd;

        HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);
        if (handle != NULL) {
            WCHAR exe_name[1024] = { 0 };
            char exe_name_utf8[1024] = { 0 };
            int exe_name_utf8_size = 1024;
            GetModuleFileNameExW(handle, NULL, exe_name, 1024);
            CloseHandle(handle);
            WideCharToMultiByte(CP_UTF8, 0, exe_name, sizeof(exe_name), exe_name_utf8, exe_name_utf8_size, NULL, NULL);
		    capture->exeFullName.append(exe_name_utf8);
        }
		windowList.push_back(capture);

		return TRUE;
	}

protected:
	std::list<CaptureEntity *> windowList;

private:

};

BOOL CALLBACK EnumerationCallbackWrapper(HWND hWnd, LPARAM lParam) {
	GetWindowListWorker * worker = (GetWindowListWorker *) lParam;
	return worker->EnumerationCallback(hWnd);
}


NAN_METHOD(getWindowList) {
	Callback *callback = new Callback(info[0].As<Function>());
	AsyncQueueWorker(new GetWindowListWorker(callback));
}