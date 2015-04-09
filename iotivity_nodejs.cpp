#include <uv.h>
#include <node.h>
#include <v8.h>
#include <nan.h>

using namespace v8;

NAN_METHOD(version) {
  NanScope();
  NanReturnValue(NanNew("0.1.0"));
}

void init(Handle<Object> exports) {
  NanScope();
  exports->Set(
    NanNew("version"),
    NanNew<FunctionTemplate>(version)->GetFunction()
  );
}

NODE_MODULE(iotivity_nodejs, init)
