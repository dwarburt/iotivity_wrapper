#include <uv.h>
#include <node.h>
#include <v8.h>
#include <nan.h>
#include <queue>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include "csdkWrapper.h"

#define EXPORT_FUNCTION(cb) exports->Set(NanNew("cb"), NanNew<FunctionTemplate>(cb)->GetFunction())

using namespace v8;

static uv_mutex_t s_callbackQueueMutex;
static uv_async_t s_notifyJs;
static std::queue<CsdkWrapper::EntityHandlerInfo *> s_callbackQueue;
static NanCallback *s_cb;
static std::thread *s_iotivityWorker;
static bool s_quitFlag = false;
static CsdkWrapper *s_wrapper;

CsdkWrapper::EntityHandlerResult entityHandlerCallback(CsdkWrapper::EntityHandlerInfo *request)
{
  /*
   * Make a copy of request.  When this callback returns iotivity will release the memory
   * that request points to.
   */
  CsdkWrapper::EntityHandlerInfo *queRequest = new CsdkWrapper::EntityHandlerInfo(*request);
  for (unsigned int i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
  {
    queRequest->params[i] = request->params[i];
  }

  /*
   * Save our copy in a queue to be processed by the main loop.  Control access to the queue
   * with our libuv mutex dedicated to this purpose.
   */
  uv_mutex_lock(&s_callbackQueueMutex);
  s_callbackQueue.push(queRequest);
  uv_mutex_unlock(&s_callbackQueueMutex);

  /*
   * Notify the libuv main loop that there is work to be done and it needs to call our handler.
   */
  uv_async_send(&s_notifyJs);

  return CsdkWrapper::EH_RESULT_SLOW;
}

NAN_METHOD(respond)
{
  NanScope();
  CsdkWrapper::EntityHandlerInfo responseInfo;
  responseInfo.resource = std::string(*NanUtf8String(args.This()->Get(NanNew("resource"))));
  responseInfo.method   = std::string(*NanUtf8String(args.This()->Get(NanNew("method"))));
  responseInfo.requestHandle = (void *)NanUInt32OptionValue(args.This(), NanNew("requestHandle"), 0);

  for (uint8_t i = 0; i < CsdkWrapper::NUM_PARAMS; i++) {
    responseInfo.params[i] = std::string(*NanUtf8String(args.This()->Get(i)));
  }
  s_wrapper->respond(&responseInfo);
  NanReturnUndefined();
}

void pushUp(CsdkWrapper::EntityHandlerInfo *cbev)
{
  /*
   * Declare JS objects.  We have one Object and one Array.
   */

  Local<Object> data = NanNew<Object>();
  Local<Object> params = NanNew<Array>();

  /*
   * Fill our array with params, these are the request params from iotivity.
   */
  for (uint8_t i = 0; i < CsdkWrapper::NUM_PARAMS; i++)
  {
    params->Set(i, NanNew(cbev->params[i]));
  }

  /*
   * Set the properties of the Object.
   */

  data->Set(NanNew("resource"), NanNew(cbev->resource));
  data->Set(NanNew("method"  ), NanNew(cbev->method));
  data->Set(NanNew("requestHandle"  ), NanNew(cbev->requestHandle));
  data->Set(NanNew("params"  ), params);
  data->Set(NanNew("respond" ), NanNew<FunctionTemplate>(respond)->GetFunction());

  /*
   * Execute the callback with our data object as the parameter.
   */
  Local<Value> argv[1] = { data };
  s_cb->Call(1, argv);
}

/*
 * This function runs on the main event loop of libuv.
 */
void notifyJsNow(uv_async_t *handle, int /*status UNUSED*/)
{

  /*
   * lock the queue mutex
   */
  uv_mutex_lock(&s_callbackQueueMutex);
  while(!s_callbackQueue.empty())
  {
    /*
     * Process one item off the queue.  The queue will be unlocked while this happens
     * because this call will go back up in JS world could take a long time to return
     * and we want other events to be able to enter the queue while this happens.
     */
    auto *cbev = s_callbackQueue.front();
    uv_mutex_unlock(&s_callbackQueueMutex);
    pushUp(cbev);
    uv_mutex_lock(&s_callbackQueueMutex);

    /*
     * Clean up this item.  Need to relock this because the 3 out of the next 4
     * statements are queue operations (pop, empty, front).
     */
    s_callbackQueue.pop();
    delete cbev;
  }
  uv_mutex_unlock(&s_callbackQueueMutex);
}

void doIotivityWork()
{
  if (!s_wrapper->start(entityHandlerCallback)) {
    std::cerr << "Unable to start!";
    return;
  }

  while (!s_quitFlag) {
    if (!s_wrapper->process()) {
      std::cerr << "Unable to process!";
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  s_wrapper->stop();
  delete s_wrapper;
}

NAN_METHOD(version) {
  NanScope();
  NanReturnValue(NanNew("0.1.0"));
}

NAN_METHOD(stop) {
  NanScope();
  s_quitFlag = true;
  NanReturnValue(NanNew("***stopping now***"));
}

NAN_METHOD(start) {
  NanScope();
  s_wrapper = new CsdkWrapper();
  if (args.Length() < 1) {
    NanThrowTypeError("invalid number of params");
  }
  s_cb = new NanCallback(args[0].As<Function>());

  s_iotivityWorker = new std::thread(doIotivityWork);
  NanReturnUndefined();
}

void init(Handle<Object> exports) {
  NanScope();
  EXPORT_FUNCTION(version);
  EXPORT_FUNCTION(stop);
  EXPORT_FUNCTION(start);

  uv_mutex_init(&s_callbackQueueMutex);
  uv_async_init(uv_default_loop(), &s_notifyJs, notifyJsNow);
}

NODE_MODULE(iotivity_nodejs, init)



