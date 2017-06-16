#pragma once
#include <nan.h>

class WinAsyncWorker :
	public Nan::AsyncWorker
{
public:
	WinAsyncWorker(Nan::Callback *callback) : Nan::AsyncWorker(callback) {};
	~WinAsyncWorker() {};
};


class CaptureEntity {

public:
	std::string id;
	std::string label;
	std::string type;
	std::string windowClassName;
	std::string windowName;
	bool antiCheat = 0;

	CaptureEntity() {};

	~CaptureEntity() {
	}
};
HRESULT RegOpen(REGSAM samDesired, HKEY * hkey);
HRESULT RegClose(HKEY hkey);
HRESULT putSZ(HKEY hkey, char * key, std::string & value);
HRESULT getSZ(HKEY hkey, char * key, std::string & value);
HRESULT putBool(HKEY hkey, char * key, bool val);
HRESULT getBool(HKEY hkey, char * key, bool * val);
const char * errno_to_text(HRESULT errorNumber);

