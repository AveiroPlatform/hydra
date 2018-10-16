#include "rcib.h"
#include "callbackinfo.h"

#include "delayed/delayed.h"
#include "ed25519/ed25519.h"
#include "hash/hash.h"

using namespace rcib;

namespace rcib {
  extern bool bterminating_;
}

extern base::LazyInstance<rcib::furOfThread> furThread_;

static void NewThread(const v8::FunctionCallbackInfo<v8::Value>& args) {
  NOTH
  DCHECK(args.IsConstructCall());
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  void * data = furThread_.Get().Wrap(args.This());
  if(data) rcib::CallbackInfo::New(args.GetIsolate(), args.This(), rcib::CallbackInfo::Free, data);
}

static void Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
  furThread_.Get().Close(static_cast<base::Thread*>(furThread_.Get().Unwrap(args.Holder())));
  furThread_.Get().Wrap(args.Holder(), (void*)(&rcib::bterminating_));
}

static void IsRunning(const v8::FunctionCallbackInfo<v8::Value>& args) {
  args.GetReturnValue().Set(v8::Boolean::New(v8::Isolate::GetCurrent(),
    furThread_.Get().IsRunning(static_cast<base::Thread*>(furThread_.Get().Unwrap(args.Holder())))));
}

static void DelayByMil(const v8::FunctionCallbackInfo<v8::Value>& args) {
  THREAD;
  DELAY_TASK_COMMON(args);
  req->w_t = TYPE_DELAY;
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayByMil, req),
    base::TimeDelta::FromMilliseconds(delayed));

  RETURN_TRUE
}

static void DelayBySec(const v8::FunctionCallbackInfo<v8::Value>& args) {
  THREAD;
  DELAY_TASK_COMMON(args);
  req->w_t = TYPE_DELAY;
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayBySec, req),
    base::TimeDelta::FromSeconds(delayed));

  RETURN_TRUE
}

static void DelayByMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
  THREAD;
  DELAY_TASK_COMMON(args);
  req->w_t = TYPE_DELAY;
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayByMin, req),
    base::TimeDelta::FromMinutes(delayed));
  RETURN_TRUE
}

static void DelayByHour(const v8::FunctionCallbackInfo<v8::Value>& args) {
  THREAD;
  DELAY_TASK_COMMON(args);
  req->w_t = TYPE_DELAY;
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayByHour, req),
    base::TimeDelta::FromHours(delayed));
  RETURN_TRUE
}

static void QueueNum(const v8::FunctionCallbackInfo<v8::Value>& args) {
  THREAD;
  uint32_t num = static_cast<uint32_t>(thr->Computational());
  args.GetReturnValue().Set(num);
}

static void Sha2(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 3
    || !args[0]->IsNumber()
    || !node::Buffer::HasInstance(args[1])
    || !args[2]->IsFunction()) {
    TYPEERROR;
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 2);
  HashData data;
  data._p = node::Buffer::Data(args[1]);
  data._plen = node::Buffer::Length(args[1]);
  req->w_t = TYPE_SHA;
  req->out = (char*)(new HashRe(&HashHelper::HashClean, thr->AsWeakPtr()));
  HashRe *hre = (HashRe *)(req->out);
  thr->message_loop()->PostTask(base::Bind(base::Unretained(HashHelper::GetInstance()),
    &HashHelper::SHA, args[0]->TOINT32(isolate), data, req));
  RETURN_TRUE
}

static void MakeKeypair(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 1
    || !node::Buffer::HasInstance(args[0])
    || node::Buffer::Length(args[0]) != 32 ) {
    TYPEERROR2(MakeKeypair requires a 32 byte buffer);
  }

  const unsigned char* seed = (unsigned char*)node::Buffer::Data(args[0]);
  v8::Local<v8::Object> privateKey = node::Buffer::New(isolate, 64).ToLocalChecked();
  unsigned char* privateKeyData = (unsigned char*)node::Buffer::Data(privateKey);
  v8::Local<v8::Object> publicKey = node::Buffer::New(isolate, 32).ToLocalChecked();
  unsigned char* publicKeyData = (unsigned char*)node::Buffer::Data(publicKey);
  for (int i = 0; i < 32; ++i) {
    privateKeyData[i] = seed[i];
  }
  crypto_sign_keypair(publicKeyData, privateKeyData);

  v8::Local<v8::Object> result = v8::Object::New(isolate);
  result->Set(v8::String::NewFromUtf8(isolate, "publicKey"), publicKey);
  result->Set(v8::String::NewFromUtf8(isolate, "privateKey"), privateKey);
  args.GetReturnValue().Set(result);
}

static void Sign(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 3
    || !node::Buffer::HasInstance(args[0])
    || !(node::Buffer::HasInstance(args[1]) || args[1]->IsObject())
    || !args[2]->IsFunction()) {
    TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback));
  }
  Ed25519Data data;
  data._msg = (unsigned char*)node::Buffer::Data(args[0]);
  data._mlen = node::Buffer::Length(args[0]);
  if(args[1]->IsObject() && !node::Buffer::HasInstance(args[1])) {
    v8::Local<v8::Value> pKey = args[1]->ToObject()->Get(v8::String::NewFromUtf8(isolate, "privateKey"));
    if (!pKey->IsObject()) {
      TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback));
    }
    v8::Local<v8::Value> privateKeyBuffer = pKey->ToObject();
    if (!node::Buffer::HasInstance(privateKeyBuffer) || 64 != node::Buffer::Length(privateKeyBuffer)) {
      TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback));
    }
    data._privateKey = (unsigned char*)node::Buffer::Data(privateKeyBuffer);
  } else if (32 == node::Buffer::Length(args[1])) {
    data._seed = (unsigned char*)node::Buffer::Data(args[1]);
  } else if (64 == node::Buffer::Length(args[1])) {
    data._privateKey = (unsigned char*)node::Buffer::Data(args[1]);
  } else {
    TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback));
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 2);
  req->w_t = TYPE_ED25519;
  req->out = (char*)(new Ed25519Re(thr->AsWeakPtr(), Ed25519Re::SIGN));
  thr->message_loop()->PostTask(base::Bind(base::Unretained(Ed25519Helper::GetInstance()),
    &Ed25519Helper::Sign, data, req));

  RETURN_TRUE
}

static void SignSync(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 2
    || !node::Buffer::HasInstance(args[0])
    || !(node::Buffer::HasInstance(args[1]) || args[1]->IsObject())) {
    TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }));
  }
  Ed25519Data data;
  data._msg = (unsigned char*)node::Buffer::Data(args[0]);
  data._mlen = node::Buffer::Length(args[0]);
  if (args[1]->IsObject() && !node::Buffer::HasInstance(args[1])) {
    v8::Local<v8::Value> pKey = args[1]->ToObject()->Get(v8::String::NewFromUtf8(isolate, "privateKey"));
    if (!pKey->IsObject()) {
      TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }));
    }
    v8::Local<v8::Value> privateKeyBuffer = pKey->ToObject();
    if (!node::Buffer::HasInstance(privateKeyBuffer) || 64 != node::Buffer::Length(privateKeyBuffer)) {
      TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }));
    }
    data._privateKey = (unsigned char*)node::Buffer::Data(privateKeyBuffer);
  } else if (32 == node::Buffer::Length(args[1])) {
    data._seed = (unsigned char*)node::Buffer::Data(args[1]);
  } else if (64 == node::Buffer::Length(args[1])) {
    data._privateKey = (unsigned char*)node::Buffer::Data(args[1]);
  } else {
    TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }));
  }

  unsigned char publicKeyData[32];
  unsigned char privateKeyData[64];
  unsigned char * privateKey = nullptr;
  if (data._seed) {
    for (int i = 0; i < 32; ++i) {
      privateKeyData[i] = data._seed[i];
    }
    crypto_sign_keypair(publicKeyData, privateKeyData);
    privateKey = privateKeyData;
  } else {
    privateKey = data._privateKey;
  }
  unsigned long long sigLen = data._mlen + 64;
  unsigned char *signatureMessageData = (unsigned char*)malloc(sigLen);
  crypto_sign(signatureMessageData, &sigLen, data._msg, data._mlen, privateKey);
  for (int i = 0; i < 64; ++i) {
    privateKeyData[i] = signatureMessageData[i];
  }
  free(signatureMessageData);
  args.GetReturnValue().Set(node::Encode(isolate, reinterpret_cast<char *>(privateKeyData), 64, node::encoding::BUFFER));
}

static void Verify(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  Ed25519Data data;
  if (args.Length() != 4
    || !node::Buffer::HasInstance(args[0])
    || !node::Buffer::HasInstance(args[1])
    || !node::Buffer::HasInstance(args[2])
    || !args[3]->IsFunction()) {
    TYPEERROR2(Verify requires(Buffer, Buffer(64), Buffer(32), callback));
  }
  if (!( 64 == node::Buffer::Length(args[1]) && 32 == node::Buffer::Length(args[2]) )) {
    TYPEERROR2(Verify requires(Buffer, Buffer(64), Buffer(32), callback))
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 3);
  data._msg = (unsigned char*)node::Buffer::Data(args[0]);
  data._mlen = node::Buffer::Length(args[0]);
  data._seed = (unsigned char*)node::Buffer::Data(args[1]);
  data._privateKey = (unsigned char*)node::Buffer::Data(args[2]); // here is pub
  req->w_t = TYPE_ED25519;
  req->out = (char*)(new Ed25519Re(thr->AsWeakPtr(), Ed25519Re::VERIFY));
  thr->message_loop()->PostTask(base::Bind(base::Unretained(Ed25519Helper::GetInstance()),
    &Ed25519Helper::Verify, data, req));

  RETURN_TRUE
}

static void VerifySync(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  Ed25519Data data;
  if (args.Length() != 3
    || !node::Buffer::HasInstance(args[0])
    || !node::Buffer::HasInstance(args[1])
    || !node::Buffer::HasInstance(args[2])) {
    TYPEERROR2(Verify requires(Buffer, Buffer(64), Buffer(32)));
  }
  if (!(64 == node::Buffer::Length(args[1]) && 32 == node::Buffer::Length(args[2]))) {
    TYPEERROR2(Verify requires(Buffer, Buffer(64), Buffer(32)))
  }

  data._msg = (unsigned char*)node::Buffer::Data(args[0]);
  data._mlen = node::Buffer::Length(args[0]);
  data._seed = (unsigned char*)node::Buffer::Data(args[1]);
  data._privateKey = (unsigned char*)node::Buffer::Data(args[2]); // here is pub

  bool relt = (crypto_sign_verify(data._seed, data._msg, data._mlen, data._privateKey) == 0);
  args.GetReturnValue().Set(relt);
}

void Terminate(void *) {
  RcibHelper::GetInstance()->Terminate();
}

template <typename TypeName>
inline void NODE_CREATE_FUNCTION(const TypeName& target) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
      NewThread);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(v8::String::NewFromUtf8(isolate, "THREAD"));

    NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(t, "isRunning", IsRunning);
    NODE_SET_PROTOTYPE_METHOD(t, "delayByMil", DelayByMil);
    NODE_SET_PROTOTYPE_METHOD(t, "delayBySec", DelayBySec);
    NODE_SET_PROTOTYPE_METHOD(t, "delayByMin", DelayByMin);
    NODE_SET_PROTOTYPE_METHOD(t, "delayByHour", DelayByHour);
    NODE_SET_PROTOTYPE_METHOD(t, "queNum", QueueNum);
    NODE_SET_PROTOTYPE_METHOD(t, "sha2", Sha2);
    NODE_SET_PROTOTYPE_METHOD(t, "sign", Sign);
    NODE_SET_PROTOTYPE_METHOD(t, "verify", Verify);

    target->Set(v8::String::NewFromUtf8(isolate, "THREAD")
      , t->GetFunction());
  }

#define NODE_CREATE_FUNCTION NODE_CREATE_FUNCTION

void Init(v8::Local<v8::Object> target) {
  NODE_CREATE_FUNCTION(target);
  NODE_SET_METHOD(target, "makeKeypair", MakeKeypair);
  NODE_SET_METHOD(target, "signSync", SignSync);
  NODE_SET_METHOD(target, "verifySync", VerifySync);
  RcibHelper::GetInstance()->Init();
  node::AtExit(Terminate);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
