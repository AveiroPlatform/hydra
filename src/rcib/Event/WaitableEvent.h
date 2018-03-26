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

#ifndef MUL_WAITABLE_EVENT_
#define MUL_WAITABLE_EVENT_

#ifndef _WIN32
#include <list>
#include "../ref_counted.h"
#include "../util_tools.h"
#endif

namespace base {

  class TimeDelta;

  class WaitableEvent {
  public:
    WaitableEvent(bool manual_reset, bool initially_signaled);
#ifdef _WIN32 
    explicit WaitableEvent(HANDLE event_handle);
    HANDLE Release();
#endif
    ~WaitableEvent();
    // Put the event in the un-signaled state.
    void Reset();

    // Put the event in the signaled state.  Causing any thread blocked on Wait
    // to be woken up.
    void Signal();

    // Wait indefinitely for the event to be signaled.
    void Wait();

    bool TimedWait(const TimeDelta& max_time);

#ifdef _WIN32
    HANDLE handle() const { return handle_; }

  private:
    HANDLE handle_;
#else
    // This is a private helper class. It's here because it's used by friends of
    // this class (such as WaitableEventWatcher) to be able to enqueue elements
    // of the wait-list
    class Waiter {
    public:
      // Signal the waiter to wake up.
      //
      // Consider the case of a Waiter which is in multiple WaitableEvent's
      // wait-lists. Each WaitableEvent is automatic-reset and two of them are
      // signaled at the same time. Now, each will wake only the first waiter in
      // the wake-list before resetting. However, if those two waiters happen to
      // be the same object (as can happen if another thread didn't have a chance
      // to dequeue the waiter from the other wait-list in time), two auto-resets
      // will have happened, but only one waiter has been signaled!
      //
      // Because of this, a Waiter may "reject" a wake by returning false. In
      // this case, the auto-reset WaitableEvent shouldn't act as if anything has
      // been notified.
      virtual bool Fire(WaitableEvent* signaling_event) = 0;

      // Waiters may implement this in order to provide an extra condition for
      // two Waiters to be considered equal. In WaitableEvent::Dequeue, if the
      // pointers match then this function is called as a final check. See the
      // comments in ~Handle for why.
      virtual bool Compare(void* tag) = 0;

    protected:
      virtual ~Waiter() {}
    };

    struct WaitableEventKernel :
      public RefCountedThreadSafe<WaitableEventKernel> {
    public:
      WaitableEventKernel(bool manual_reset, bool initially_signaled);
      bool Dequeue(Waiter* waiter, void* tag);
      const bool manual_reset_;
      bool signaled_;
      CriticalSection lock_;
      std::list<Waiter*> waiters_;

    private:
      friend class RefCountedThreadSafe<WaitableEventKernel>;
      ~WaitableEventKernel();
    };

  private:
    bool SignalAll();
    bool SignalOne();
    void Enqueue(Waiter* waiter);

    scoped_refptr<WaitableEventKernel> kernel_;

#endif
  };
} //end base

#endif
