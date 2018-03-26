#ifndef CALLBACKINFO_
#define CALLBACKINFO_

namespace rcib {

  typedef void(*FreeCallback)(void* data, void* hint);

  class CallbackInfo {
    public:
      static inline void Free(void* data, void* hint);
      static inline CallbackInfo* New(v8::Isolate* isolate,
        v8::Handle<v8::Object> object,
        FreeCallback callback,
        void* data,
        void* hint = 0);
    private:
      static void WeakCallback(const v8::WeakCallbackInfo<CallbackInfo>&);
      inline void WeakCallback(v8::Isolate* isolate);
      inline CallbackInfo(v8::Isolate* isolate,
        v8::Handle<v8::Object> object,
        FreeCallback callback,
        void* data,
        void* hint);
      ~CallbackInfo();
      v8::Persistent<v8::Object> persistent_;
      FreeCallback const callback_;
      void* const data_;
      void* const hint_;
      DISALLOW_COPY_AND_ASSIGN_(CallbackInfo);
  };
  //static
  void CallbackInfo::Free(void* data, void*) {
    base::Thread *thr = static_cast<base::Thread*>(data);
    if (!thr) return;
    delete thr;
  }
  //static
  CallbackInfo* CallbackInfo::New(v8::Isolate* isolate,
    v8::Handle<v8::Object> object,
    FreeCallback callback,
    void* data,
    void* hint) {
    DCHECK_G(object->InternalFieldCount(), 0);
    return new CallbackInfo(isolate, object, callback, data, hint);
  }

  CallbackInfo::CallbackInfo(v8::Isolate* isolate,
    v8::Handle<v8::Object> object,
    FreeCallback callback,
    void* data,
    void* hint)
    : persistent_(isolate, object),
    callback_(callback),
    data_(data),
    hint_(hint) {
    persistent_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
    persistent_.MarkIndependent();
    isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(*this));
  }

  CallbackInfo::~CallbackInfo() {
    persistent_.Reset();
  }
  //static
  void CallbackInfo::WeakCallback(
    const v8::WeakCallbackInfo<CallbackInfo>& data) {
    CallbackInfo* self = data.GetParameter();
    self->WeakCallback(data.GetIsolate());
    delete self;
  }

  void CallbackInfo::WeakCallback(v8::Isolate* isolate) {
    callback_(data_, hint_);
    int64_t change_in_bytes = -static_cast<int64_t>(sizeof(*this));
    isolate->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
  }

} // end namespace

#endif
