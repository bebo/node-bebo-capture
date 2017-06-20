#pragma once
#include <nan.h>
#include <cstdint>
#include <cinttypes>

class WinAsyncWorker :
	public Nan::AsyncWorker
{
public:
	WinAsyncWorker(Nan::Callback *callback) : Nan::AsyncWorker(callback) {};
	~WinAsyncWorker() {};
protected:
	bool chk(HRESULT hresult, std::string & msg);
	bool chk(HRESULT hresult, const char * msg);
};


class CaptureEntity {

public:
	std::string id;
	std::string label;
	std::string type;
	std::string windowClassName;
	std::string windowName;
	bool antiCheat = 0;
	bool once = 0;
        uint64_t hwnd = 0;

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
HRESULT putQWord(HKEY hkey, char * key, uint64_t val);
HRESULT getQWord(HKEY hkey, char * key, uint64_t * val);
const char * errno_to_text(HRESULT errorNumber);

