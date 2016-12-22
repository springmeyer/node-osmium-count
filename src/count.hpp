#pragma once

#pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// #pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

NAN_METHOD(count);
void AsyncCount(uv_work_t* req);
void AfterCount(uv_work_t* req);
