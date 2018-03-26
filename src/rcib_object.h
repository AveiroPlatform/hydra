#ifndef RCIB_OBJECT_
#define RCIB_OBJECT_

#include <node.h>
#include <v8.h>
#include <uv.h>
#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <node_buffer.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include "rcib/macros.h"
#include "rcib/aligned_memory.h"
#include "rcib/lazy_instance.h"
#include "rcib/ref_counted.h"
#include "rcib/WrapperObj.h"
#include "rcib/WeakPtr.h"
#include "rcib/FastDelegateImpl.h"
#include "rcib/time/time.h"
#include "rcib/MessagePump.h"
#include "rcib/util_tools.h"
#include "rcib/Event/WaitableEvent.h"
#include "rcib/PendingTask.h"
#include "rcib/observer_list.h"
#include "rcib/MessagePumpDefault.h"
#include "rcib/MessageLoop.h"
#include "rcib/roler.h"
#include "rcib/Thread.h"
#include "rcib/at_exist.h"

namespace rcib {

  struct async_t_handle {
    uv_async_t handle_;
  };

  enum WORKTYPE {
    TYPE_START = 0,
    TYPE_SHA,
    TYPE_ED25519,
    TYPE_DELAY,
    //...
    TYPE_END
  };

  class async_req {
  public:
    async_req() {
      out = NULL;
      isolate = NULL;
      result = 0;
    }
    std::string error;
    char *out;
    ssize_t result;
    v8::Isolate* isolate;
    v8::Persistent<v8::Function> callback;
    bool finished;
    WORKTYPE w_t;
  };

  /*A thread - safe allocator
  */
  class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  public:
    virtual void* Allocate(size_t length);
    virtual void* AllocateUninitialized(size_t length);
    virtual void Free(void* data, size_t);
  };

  class Param {
  public:
    Param() {}
    virtual ~Param(){}
  };

  class RcibHelper {
  public:
    explicit RcibHelper();
    //static
    static RcibHelper* GetInstance();
    static void AfterAsync(uv_async_t* h);
    static void DoNopAsync(async_req* r);
    static void EMark(async_req* req, std::string message);
    static void EMark2(async_req* req, std::string message);
    static void init_async_req(async_req *req);

    void Init();
    void Terminate();

    void Uv_Send(async_req* req, uv_async_t* h);
    inline void Push(async_req *itme) {
      pending_queue_.push_back(itme);
    }
    inline async_req* Pop() {
      if (working_queue_.empty())
        return NULL;
      async_req * item = working_queue_.front();
      working_queue_.pop_front();
      return item;
    }
    inline void PickFinished();
    inline size_t taskNum() {
      return 0;
    }

  private:
    std::list<async_req *> pending_queue_;
    std::list<async_req *> working_queue_;
    async_t_handle *handle_;
  };

  class furOfThread {
  public:
    furOfThread() {
    }
    ~furOfThread() {
    }

    void* Wrap(v8::Local<v8::Object> object) {
      DCHECK_EQ(false, object.IsEmpty());
      DCHECK_G(object->InternalFieldCount(), 0);
      base::Thread *thread = new base::Thread();
      if (!thread) return nullptr;
      if (!thread->IsRunning()) {
        thread->set_thread_name("distribute_task_thread");
        thread->StartWithOptions(base::Thread::Options());
      }
      object->SetAlignedPointerInInternalField(0, (void*)thread);
      return thread;
    }

    void Wrap(v8::Local<v8::Object> object, void *tmp) {
      DCHECK_EQ(false, object.IsEmpty());
      DCHECK_G(object->InternalFieldCount(), 0);
      object->SetAlignedPointerInInternalField(0, nullptr);
    }

    void *Unwrap(v8::Local<v8::Object> object) {
      DCHECK_EQ(false, object.IsEmpty());
      DCHECK_G(object->InternalFieldCount(), 0);
      return object->GetAlignedPointerFromInternalField(0);
    }

    bool IsRunning(base::Thread* thread) {
      if (thread) {
        return thread->IsRunning();
      }
      return false;
    }

    void Close(base::Thread* thread) {
      if (thread) {
        delete thread;
      }
    }

    base::Thread* Get(v8::Local<v8::Object> object) {
      return static_cast<base::Thread*>(Unwrap(object));
    }
  };

} //end rcib

#endif
