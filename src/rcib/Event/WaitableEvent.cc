/* Copyright chromium  All rights reserved.
 *
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

#if !defined(_WIN32)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <list>
#include <string>
#include "../macros.h"
#endif
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "../time/time.h"
#include "WaitableEvent.h"

namespace base {

#ifdef _WIN32
  WaitableEvent::WaitableEvent(bool manual_reset, bool signaled)
    : handle_(CreateEvent(NULL, manual_reset, signaled, NULL)) {
      // We're probably going to crash anyways if this is ever NULL, so we might as
      // well make our stack reports more informative by crashing here.
      assert(handle_);
  }

  WaitableEvent::WaitableEvent(HANDLE event_handle)
    : handle_(event_handle){
      assert(handle_ != INVALID_HANDLE_VALUE);
  }

  HANDLE WaitableEvent::Release() {
    HANDLE rv = handle_;
    handle_ = NULL;
    return rv;
  }

  WaitableEvent::~WaitableEvent() {
    assert(handle_);
    CloseHandle(handle_);
    handle_ = NULL;
  }

  void WaitableEvent::Reset() {
    ResetEvent(handle_);
  }

  void WaitableEvent::Signal() {
    SetEvent(handle_);
  }

  // Wait indefinitely for the event to be signaled.
  void WaitableEvent::Wait() {
    DWORD result = WaitForSingleObject(handle_, INFINITE);
  }

  bool WaitableEvent::TimedWait(const TimeDelta& max_time) {
    double timeout = ceil(max_time.InMillisecondsF());
    DWORD result = WaitForSingleObject(handle_, static_cast<DWORD>(timeout));
    switch (result) {
    case WAIT_OBJECT_0:
      return true;
    case WAIT_TIMEOUT:
      return false;
    }
    // It is most unexpected that this should ever fail.  Help consumers learn
    // about it if it should ever fail.
    assert(0);
    return false;
  }
#else

  //linux
  WaitableEvent::WaitableEventKernel::WaitableEventKernel(bool manual_reset,
    bool initially_signaled)
    : manual_reset_(manual_reset),
    signaled_(initially_signaled),
    lock_() {
  }

  WaitableEvent::WaitableEventKernel::~WaitableEventKernel() {
  }

  // -----------------------------------------------------------------------------
  // Remove a waiter from the list of those waiting. Return true if the waiter was
  // actually removed. Called with lock held.
  // -----------------------------------------------------------------------------
  bool WaitableEvent::WaitableEventKernel::Dequeue(Waiter* waiter, void* tag) {
    for (std::list<Waiter*>::iterator
      i = waiters_.begin(); i != waiters_.end(); ++i) {
        if (*i == waiter && (*i)->Compare(tag)) {
          waiters_.erase(i);
          return true;
        }
    }
    return false;
  }

  WaitableEvent::WaitableEvent(bool manual_reset, bool initially_signaled)
    : kernel_(new WaitableEventKernel(manual_reset, initially_signaled)) {
  }

  WaitableEvent::~WaitableEvent() {
  }

  void WaitableEvent::Reset() {
    AutoCritSecLock<CriticalSection> locked(kernel_->lock_, true);
    kernel_->signaled_ = false;
  }

  void WaitableEvent::Signal() {
    AutoCritSecLock<CriticalSection> locked(kernel_->lock_, true);

    if (kernel_->signaled_)
      return;

    if (kernel_->manual_reset_) {
      SignalAll();
      kernel_->signaled_ = true;
    } else {
      // In the case of auto reset, if no waiters were woken, we remain
      // signaled.
      if (!SignalOne())
        kernel_->signaled_ = true;
    }
  }

  // -----------------------------------------------------------------------------
  // This is a synchronous waiter. The thread is waiting on the given condition
  // variable and the fired flag in this object.
  // -----------------------------------------------------------------------------
  class SyncWaiter : public WaitableEvent::Waiter {
  public:
    SyncWaiter()
      : fired_(false),
      signaling_event_(NULL),
      lock_(),
      cv_(&lock_) {
    }

    virtual bool Fire(WaitableEvent* signaling_event) OVERRIDE {
      AutoCritSecLock<CriticalSection> locked(lock_, true);

      if (fired_)
        return false;

      fired_ = true;
      signaling_event_ = signaling_event;

      cv_.Broadcast();

      // Unlike AsyncWaiter objects, SyncWaiter objects are stack-allocated on
      // the blocking thread's stack.  There is no |delete this;| in Fire.  The
      // SyncWaiter object is destroyed when it goes out of scope.

      return true;
    }

    WaitableEvent* signaling_event() const {
      return signaling_event_;
    }

    // ---------------------------------------------------------------------------
    // These waiters are always stack allocated and don't delete themselves. Thus
    // there's no problem and the ABA tag is the same as the object pointer.
    // ---------------------------------------------------------------------------
    virtual bool Compare(void* tag) OVERRIDE {
      return this == tag;
    }

    // ---------------------------------------------------------------------------
    // Called with lock held.
    // ---------------------------------------------------------------------------
    bool fired() const {
      return fired_;
    }

    // ---------------------------------------------------------------------------
    // During a TimedWait, we need a way to make sure that an auto-reset
    // WaitableEvent doesn't think that this event has been signaled between
    // unlocking it and removing it from the wait-list. Called with lock held.
    // ---------------------------------------------------------------------------
    void Disable() {
      fired_ = true;
    }

    CriticalSection* lock() {
      return &lock_;
    }

    ConditionVariable* cv() {
      return &cv_;
    }

  private:
    bool fired_;
    WaitableEvent* signaling_event_;  // The WaitableEvent which woke us
    CriticalSection lock_;
    ConditionVariable cv_;
  };

  void WaitableEvent::Wait() {
    bool result = TimedWait(TimeDelta::FromSeconds(-1));
  }

  bool WaitableEvent::TimedWait(const TimeDelta& max_time) {
    const TimeTicks end_time(TimeTicks::Now() + max_time);
    const bool finite_time = max_time.ToInternalValue() >= 0;

    kernel_->lock_.Lock();
    if (kernel_->signaled_) {
      if (!kernel_->manual_reset_) {
        // In this case we were signaled when we had no waiters. Now that
        // someone has waited upon us, we can automatically reset.
        kernel_->signaled_ = false;
      }

      kernel_->lock_.UnLock();
      return true;
    }

    SyncWaiter sw;
    sw.lock()->Lock();

    Enqueue(&sw);
    kernel_->lock_.UnLock();
    // We are violating locking order here by holding the SyncWaiter lock but not
    // the WaitableEvent lock. However, this is safe because we don't lock @lock_
    // again before unlocking it.

    for (;;) {
      const TimeTicks current_time(TimeTicks::Now());

      if (sw.fired() || (finite_time && current_time >= end_time)) {
        const bool return_value = sw.fired();

        // We can't acquire @lock_ before releasing the SyncWaiter lock (because
        // of locking order), however, in between the two a signal could be fired
        // and @sw would accept it, however we will still return false, so the
        // signal would be lost on an auto-reset WaitableEvent. Thus we call
        // Disable which makes sw::Fire return false.
        sw.Disable();
        sw.lock()->UnLock();

        kernel_->lock_.Lock();
        kernel_->Dequeue(&sw, &sw);
        kernel_->lock_.UnLock();

        return return_value;
      }

      if (finite_time) {
        const TimeDelta max_wait(end_time - current_time);
        sw.cv()->TimedWait(max_wait);
      } else {
        sw.cv()->Wait();
      }
    } // end for
  }

  // -----------------------------------------------------------------------------
  // Add a waiter to the list of those waiting. Called with lock held.
  // -----------------------------------------------------------------------------
  void WaitableEvent::Enqueue(Waiter* waiter) {
    kernel_->waiters_.push_back(waiter);
  }

  // -----------------------------------------------------------------------------
  // Wake all waiting waiters. Called with lock held.
  // -----------------------------------------------------------------------------
  bool WaitableEvent::SignalAll() {
    bool signaled_at_least_one = false;

    for (std::list<Waiter*>::iterator
      i = kernel_->waiters_.begin(); i != kernel_->waiters_.end(); ++i) {
        if ((*i)->Fire(this))
          signaled_at_least_one = true;
    }

    kernel_->waiters_.clear();
    return signaled_at_least_one;
  }

  // ---------------------------------------------------------------------------
  // Try to wake a single waiter. Return true if one was woken. Called with lock
  // held.
  // ---------------------------------------------------------------------------
  bool WaitableEvent::SignalOne() {
    for (;;) {
      if (kernel_->waiters_.empty())
        return false;

      const bool r = (*kernel_->waiters_.begin())->Fire(this);
      kernel_->waiters_.pop_front();
      if (r)
        return true;
    }
  }

#endif
} //end base
