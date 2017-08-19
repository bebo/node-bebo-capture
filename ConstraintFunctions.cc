#include "ConstraintFunctions.h"
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

class GetConstraintWorker : public WinAsyncWorker
{
  public:
	GetConstraintWorker(Callback *callback)
		: WinAsyncWorker(callback){};
	~GetConstraintWorker()
	{
		if (constraint)
		{
			delete constraint;
		}
	};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute()
	{
		HKEY hkey = NULL;
		if (!chk(RegOpen(KEY_READ, &hkey), "Can't open registry"))
		{
			return;
		}
		this->readData(hkey);
		chk(RegClose(hkey), "Can't close registry");
	}

	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback()
	{
		HandleScope scope;

		Local<Object> obj = Nan::New<Object>();
		Set(obj, New("width").ToLocalChecked(), New(constraint->width));
		Set(obj, New("height").ToLocalChecked(), New(constraint->height));
		Set(obj, New("fps").ToLocalChecked(), New(constraint->fps));

		Local<Value> argv[] = {
			Null(), obj};

		callback->Call(2, argv);
	}

  protected:
	ConstraintEntity *constraint;

	bool chk(HRESULT hresult, std::string &msg)
	{
		std::string mymsg(msg);
		if (hresult != NOERROR)
		{
			mymsg.append(errno_to_text(hresult));
			SetErrorMessage(mymsg.c_str());
			return false;
		}
		return true;
	}

	bool chk(HRESULT hresult, const char *msg)
	{
		return chk(hresult, std::string(msg));
	}

	bool chkGetDWord(HKEY hkey, char *key, uint32_t *data)
	{
		std::string msg("Can't read from registry (");
		msg.append(key);
		msg.append(") ");
		return chk(getDWord(hkey, key, data), msg);
	}

	bool chkPutDWord(HKEY hkey, char *key, uint32_t data)
	{
		std::string msg("Can't write to registry (");
		msg.append(key);
		msg.append(") ");
		return chk(putDWord(hkey, key, data), msg);
	}

	void readData(HKEY hkey)
	{
		constraint = new ConstraintEntity();
		if (!chkGetDWord(hkey, "CaptureWidth", &constraint->width))
			return;
		if (!chkGetDWord(hkey, "CaptureHeight", &constraint->height))
			return;
		if (!chkGetDWord(hkey, "CaptureFPS", &constraint->fps))
			return;
	}
};

class SetConstraintWorker : public GetConstraintWorker
{
  public:
	SetConstraintWorker(ConstraintEntity *constraint, Callback *callback)
		: GetConstraintWorker(callback)
	{
		this->constraint = constraint;
	};
	~SetConstraintWorker(){};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute()
	{

		HKEY hkey = NULL;
		if (!chk(RegOpen(KEY_ALL_ACCESS, &hkey), "Can't open registry for write"))
		{
			return;
		}

		if (constraint->width != 0)
		{
			if (!chkPutDWord(hkey, "CaptureWidth", constraint->width))
				return;
		}

		if (constraint->height != 0)
		{
			if (!chkPutDWord(hkey, "CaptureHeight", constraint->height))
				return;
		}

		if (constraint->fps != 0)
		{
			if (!chkPutDWord(hkey, "CaptureFPS", constraint->fps))
				return;
		}

		delete (constraint);
		readData(hkey);
		chk(RegClose(hkey), "Can't close registry");
	}
};

NAN_METHOD(getConstraints)
{
	Callback *callback = new Callback(info[0].As<Function>());
	AsyncQueueWorker(new GetConstraintWorker(callback));
}

NAN_METHOD(setConstraints)
{
	auto constraint = new ConstraintEntity();
	Nan::Maybe<uint32_t> width = Nan::To<uint32_t>(info[0]);
	constraint->width = width.ToChecked();

	Nan::Maybe<uint32_t> height = Nan::To<uint32_t>(info[1]);
	constraint->height = height.ToChecked();

	Nan::Maybe<uint32_t> fps = Nan::To<uint32_t>(info[2]);
	constraint->fps = fps.ToChecked();

	Callback *callback = new Callback(info[3].As<Function>());
	AsyncQueueWorker(new SetConstraintWorker(constraint, callback));
}
