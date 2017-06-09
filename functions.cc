#include "functions.h"
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
  char * id;
  char * label;
  char * type;
};

class GetCaptureWorker: public AsyncWorker {
public:
	GetCaptureWorker(Callback *callback)
		: AsyncWorker(callback) {};
	~GetCaptureWorker() {};

	// Executed inside the worker-thread.
	// It is not safe to access V8, or V8 data structures
	// here, so everything we need for input and output
	// should go on `this`.
	void Execute() {
		capture = new CaptureEntity();
		capture->id = "desktop:0";
		capture->label = "Screen:0";
		capture->type = "desktop";
	}

	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback() {
		HandleScope scope;

		Local<Object> obj = Nan::New<Object>();
		Set(obj, New("id").ToLocalChecked(), New(capture->id).ToLocalChecked());
		Set(obj, New("label").ToLocalChecked(),New(capture->label).ToLocalChecked());
		Set(obj, New("type").ToLocalChecked(), New(capture->type).ToLocalChecked());

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