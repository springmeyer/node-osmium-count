#include "count.hpp"

extern "C" {

static void init(v8::Local<v8::Object> target) {
    Nan::HandleScope scope;
    Nan::SetMethod(target, "count", count);
}

#define MAKE_MODULE(_modname) NODE_MODULE(_modname, init);
MAKE_MODULE(MODULE_NAME);

}