/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

 #include <v8.h>
 #ifdef _WIN32
 #include <windows.h>
 #include <objbase.h>
 #include <shlwapi.h>
 #pragma comment(lib, "shlwapi.lib")
 #pragma comment(lib, "ole32.lib")
 #else
 #include <errno.h>
 #include <pthread.h>
 #include <unistd.h>
 #include <assert.h>
 #endif
 #ifndef _WIN32
 #if defined(__APPLE__) && defined(__MACH__)
 #include <sys/resource.h>
 #else
 #include <limits.h>
 #include <sys/syscall.h>
 #endif
 #endif

#include <string>
#include <memory>
#include <list>
#include <queue>
#include <map>
#include "macros.h"
#include "ref_counted.h"
#include "WrapperObj.h"
#include "WeakPtr.h"
#include "FastDelegateImpl.h"
#include "time/time.h"
#include "MessagePump.h"
#include "util_tools.h"
#include "Event/WaitableEvent.h"
#include "PendingTask.h"
#include "observer_list.h"
#include "MessagePumpDefault.h"
#include "MessageLoop.h"
#include "roler.h"
#include "Thread.h"

namespace base {
#ifdef _WIN32
  Thread::ScopedCOMInitializer::~ScopedCOMInitializer() {
    assert(GetCurrentThreadId() == thread_id_);
    CoUninitialize();
  }

  void Thread::ScopedCOMInitializer::Initialize(COINIT init) {
    thread_id_ = GetCurrentThreadId();
    CoInitializeEx(NULL, init);
  }

  DWORD __stdcall ThreadFunc(void* params);
#else
  void * ThreadFunc(void* params);
#endif
  Thread::Options::Options()
    :message_loop_type_(MessageLoop::TYPE_DEFAULT), stack_size_(0) {
  }
  Thread::Options::Options(MessageLoop::Type type, size_t size)
    :message_loop_type_(type), stack_size_(size) {

  }

  Thread::Thread():
#ifdef _WIN32
  com_status_(NONE),
#endif
    started_(false),
    stopping_(false),
    running_(false),
    thread_(NULL),
    message_loop_(NULL),
    thread_id_(kInvalidThreadId),
    startup_data_(NULL) {
    Init();
  }

  Thread::Thread(const char* name):
#ifdef _WIN32
  com_status_(NONE),
#endif
    started_(false),
    stopping_(false),
    running_(false),
    thread_(NULL),
    message_loop_(NULL),
    thread_id_(kInvalidThreadId),
    startup_data_(NULL),
    name_(name) {
    Init();
  }

  void Thread::Init(){
    computational_ = 0;
  }

  Thread::~Thread(){
    Stop();
  }

  bool Thread::StartWithOptions(const Options& options) {
    assert(!message_loop_);
    StartupData startup_data(options);
    startup_data_ = &startup_data;
    if(!CreateThreadInternal(options.stack_size_, this, &thread_)) {
      startup_data_ = NULL;
      return false;
    }
    startup_data.event_.Wait();
    startup_data_ = NULL;
    started_ = true;
    assert(message_loop_);
    return true;
  }

  //static
  bool Thread::CreateThreadInternal(size_t stack_size, Thread *thread, PlatformThreadHandle *out_thread_handle) {
#ifdef _WIN32
    unsigned int flags = 0;
    if(stack_size > 0)
      flags = STACK_SIZE_PARAM_IS_A_RESERVATION;

    Thread::ThreadParams * params = new Thread::ThreadParams;
    params->thread_ = thread;

    PlatformThreadHandle thread_handle = ::CreateThread(NULL, stack_size, ThreadFunc, params, flags, NULL);
    if(NULL == thread_handle){
      delete params;
      return false;
    }
    if(out_thread_handle){
      *out_thread_handle = thread_handle;
    }else{
      CloseHandle(thread_handle);
    }
    return true;
#else
    bool success = false;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    if (stack_size > 0)
      pthread_attr_setstacksize(&attributes, stack_size);

    ThreadParams* params = new ThreadParams;
    params->thread_ = thread;

    int err = pthread_create(out_thread_handle, &attributes, ThreadFunc, params);
    success = !err;
    if (!success) {
      errno = err;
    }
    pthread_attr_destroy(&attributes);
    if (!success) {
      delete params;
    }
    return success;
#endif
  }

  void Thread::ThreadMain() {
    std::auto_ptr<MessageLoop> message_loop;
    message_loop.reset(new MessageLoop(startup_data_->options_.message_loop_type_));
    assert(message_loop.get());
#ifdef _WIN32
    thread_id_ = GetCurrentThreadId();
#elif defined(__APPLE__) && defined(__MACH__)
    thread_id_ = pthread_mach_thread_np(pthread_self());
#else
    thread_id_ = syscall(__NR_gettid);
#endif
    message_loop->set_thread_name(name_);
    message_loop_ = message_loop.get();
#ifdef _WIN32
    std::auto_ptr<ScopedCOMInitializer> com_initializer;
    if(com_status_ != NONE) {
      com_initializer.reset((com_status_ == STA) ?
        new ScopedCOMInitializer() :
      new ScopedCOMInitializer(MTA));
    }
#endif

    running_ = true;
    startup_data_->event_.Signal();
    Run(message_loop_);

    running_ = false;
    // Let the thread do extra cleanup.
    CleanUp();
#ifdef _WIN32
    com_initializer.reset();
#endif
    message_loop_ = NULL;
  }

  void Thread::Run(MessageLoop *message_loop) {
    message_loop->Run();
  }

  bool Thread::IsRunning() const {
    return running_;
  }

  void Thread::SetPriority(ThreadPriority priority) {
#ifdef _WIN32
    switch (priority) {
    case kThreadPriority_Normal:
      ::SetThreadPriority(thread_, THREAD_PRIORITY_NORMAL);
      break;
    case kThreadPriority_RealtimeAudio:
      ::SetThreadPriority(thread_, THREAD_PRIORITY_TIME_CRITICAL);
      break;
    default:
      break;
    }
#endif
  }

  void Thread::Stop() {
    if(!started_)
      return;

    if(stopping_ || !message_loop_)
      return;

    stopping_ = true;

    message_loop_->Quit();

    PlatformThreadHandle thread_handle = thread_;
#ifdef _WIN32
    DWORD result = WaitForSingleObject(thread_handle, INFINITE);
    if(result != WAIT_OBJECT_0){
      assert(0);
    }
    CloseHandle(thread_handle);
#else
    pthread_join(thread_handle, NULL);
#endif
    started_ = false;
    stopping_ = false;
  }

  bool Thread::set_thread_name(std::string name) {
    if(name_.empty()){
      name_ = name;
      return true;
    }
    return false;
  }

  bool Thread::SetRoler(std::string name, FurRoler &ft) {
    FileIter iter = rolers_.find(name);
    if (iter == rolers_.end()){
      rolers_.insert(std::make_pair(name, ft));
      return true;
    } else{
      // cannot set twice
    }
    return false;
  }

  FurRoler Thread::GetRoler(std::string name) {
    FileIter iter = rolers_.find(name);
    if (iter == rolers_.end()){
      return FurRoler(NULL);
    }
    return iter->second;
  }

  void Thread::RemoveRoler(std::string name) {
    FileIter iter = rolers_.find(name);
    if (iter == rolers_.end()){
      return;
    }
    rolers_.erase(iter);
  }

  void Thread::IncComputational() {
    ++computational_;
  }

  void Thread::DecComputational() {
    --computational_;
  }
  size_t Thread::Computational(){
    return computational_;
  }

}//end base

namespace base {
#ifdef _WIN32
  DWORD __stdcall ThreadFunc(void* params) {
    Thread::ThreadParams *thread_params = static_cast<Thread::ThreadParams*>(params);
    base::Thread *thread = thread_params->thread_;
    delete thread_params;

    thread->ThreadMain();

    return NULL;
  }
#else
  void* ThreadFunc(void* params) {
    Thread::ThreadParams *thread_params = static_cast<Thread::ThreadParams*>(params);
    base::Thread *thread = thread_params->thread_;
    delete thread_params;

    thread->ThreadMain();

    return NULL;
  }
#endif
} //end base
