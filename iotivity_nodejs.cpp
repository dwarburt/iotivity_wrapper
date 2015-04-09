#include <uv.h>
#include <node.h>
#include <v8.h>
#include <nan.h>
#include <queue>
#include <vector>
#include "csdkWrapper.h"

using namespace v8;

static uv_mutex_t s_callbackQueueMutex;
static uv_async_t s_notifyJs;
static std::queue<CsdkWrapper::EntityHandlerInfo *> s_callbackQueue;
static NanCallback *cb;
static CsdkWrapper wrapper;
static std::string gParams[CsdkWrapper::NUM_PARAMS];

CsdkWrapper::EntityHandlerResult entityHandlerCallback(CsdkWrapper::EntityHandlerInfo *request)
{
    CsdkWrapper::EntityHandlerResult result = CsdkWrapper::EH_RESULT_ERROR;

    CsdkWrapper::EntityHandlerInfo *queRequest = new CsdkWrapper::EntityHandlerInfo();

    queRequest->method        = request->method;
    queRequeset->resource     = request->resource;
    queRequest->requestHandle = request->requestHandle;


    if (request->method == "GET")
    {
        for (unsigned int i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
        {
            queRequest->params[i] = gParams[i];
        }
        result = CsdkWrapper::EH_RESULT_OK;
    }
    else if (request->method == "PUT")
    {
        for (unsigned int i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
        {
            gParams[i] = request->params[i];
            std::cout << "param" << i + 1 << " = " << gParams[i] << std::endl;
        }
        result = CsdkWrapper::EH_RESULT_OK;
    }

    return result;
}
void notifyJsNow(uv_async_t *handle, int /*status UNUSED*/)
{
    uv_mutex_lock(&s_callbackQueueMutex);
    while(!s_callbackQueue.empty())
    {
        auto *cbev = s_callbackQueue.front();
        uv_mutex_unlock(&s_callbackQueueMutex);
        // TODO: Propigate event data to JavaScript
        //cbev->executeCallback();
        uv_mutex_lock(&s_callbackQueueMutex);
        s_callbackQueue.pop();
        delete cbev;
    }
    uv_mutex_unlock(&s_callbackQueueMutex);
}

NAN_METHOD(version) {
  NanScope();
  NanReturnValue(NanNew("0.1.0"));
}

NAN_METHOD(stop) {
  NanScope();
  NanReturnValue(NanNew("***stopping now***"));
}

NAN_METHOD(start) {
  NanScope();
  if (args.Length() < 1) {
    NanThrowTypeError("invalid number of params");
  }
  cb = new NanCallback(args[0].As<Function>());

  // TODO:  start background thread
  NanReturnUndefined();
}

NAN_METHOD(ping) {
  NanScope();
  Local<Value> argv[1] = { NanNew("hello") };

  cb->Call(1, argv);
  Local<Value> argv2[1] = { NanNew("Goodbye") };

  cb->Call(1, argv2);
  NanReturnUndefined();
}

NAN_METHOD(callback) {
  NanScope();
  if (args.Length() < 1) {
    NanThrowTypeError("invalid number of params");
  }
  cb = new NanCallback(args[0].As<Function>());
  Local<Value> argv[1] = { NanNew("hello") };

  cb->Call(1, argv);
  NanReturnUndefined();
}

void init(Handle<Object> exports) {
  NanScope();
  exports->Set(
    NanNew("version"),
    NanNew<FunctionTemplate>(version)->GetFunction()
  );
  uv_mutex_init(&s_callbackQueueMutex);
  uv_async_init(uv_default_loop(), &s_notifyJs, notifyJsNow);

  exports->Set(
    NanNew("stop"),
    NanNew<FunctionTemplate>(stop)->GetFunction()
  );

  exports->Set(
    NanNew("start"),
    NanNew<FunctionTemplate>(start)->GetFunction()
  );

  exports->Set(
    NanNew("ping"),
    NanNew<FunctionTemplate>(ping)->GetFunction()
  );

  exports->Set(
    NanNew("callback"),
    NanNew<FunctionTemplate>(callback)->GetFunction()
  );
}

NODE_MODULE(iotivity_nodejs, init)



