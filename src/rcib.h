#ifndef RCIB_
#define RCIB_

#include "rcib_object.h"

#define ISOLATE(V) v8::Isolate* isolate = (V).GetIsolate();\
  v8::HandleScope scope(isolate);

#define TYPEERROR isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "param error"))); \
  return;

#define TYPEERROR2(E) isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, ("Error: "#E)))); \
  return;

#define TOUINT32(i) ToUint32(i->GetCurrentContext()).FromMaybe(v8::Local<v8::Uint32>())->Value()

#define TOINT32(i)  ToInt32(i->GetCurrentContext()).FromMaybe(v8::Local<v8::Int32>())->Value()

#define INITHELPER(V,I)  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast((V)[(I)]);  \
  async_req* req = new async_req; \
  RcibHelper::init_async_req(req);  \
  req->isolate = isolate; \
  req->callback.Reset(isolate, callback);

#define DELAY_TASK_COMMON(V) ISOLATE(V) \
  if ((V).Length() != 2 || !(V)[0]->IsNumber() || !(V)[1]->IsFunction()) { \
    TYPEERROR \
    } \
  INT64 delayed = (V)[0]->TOUINT32(isolate); \
  INITHELPER(V, 1)

#define   NOTH     if (bterminating_) { \
                      return; \
                   }

#define   THREAD     base::Thread* thr = furThread_.Get().Get(args.Holder()); \
                     if (!thr) { \
                       args.GetReturnValue().Set(false);  \
                       return; \
                     }

#define RETURN_TRUE  args.GetReturnValue().Set(true);

#define GETATTRINUM(N, O, S) size_t N = -1;  \
   do{  \
     v8::Local<v8::Object> object = (O); \
     DCHECK_EQ(false, object.IsEmpty());  \
     v8::Local<v8::String> proto = v8::String::NewFromUtf8(isolate, (#S)); \
     v8::Local<v8::Value> value = object->Get(proto); \
     if (value->IsNumber()){  \
       N = value->TOUINT32(isolate);  \
     }  \
   }while(false);

#endif
