// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MUL_MESSAGE_PUMP_
#define MUL_MESSAGE_PUMP_

namespace base {

  class NOVTABLE MessagePump {
  public: 
    class NOVTABLE Delegate {
    public:
      // Called from within Run in response to ScheduleWork or when the message
      // pump would otherwise call DoDelayedWork.  Returns true to indicate that
      // work was done.  DoDelayedWork will still be called if DoWork returns
      // true, but DoIdleWork will not.
      virtual bool DoWork() = 0;

      // Called from within Run in response to ScheduleDelayedWork or when the
      // message pump would otherwise sleep waiting for more work.  Returns true
      // to indicate that delayed work was done.  DoIdleWork will not be called
      // if DoDelayedWork returns true.  Upon return |next_delayed_work_time|
      // indicates the time when DoDelayedWork should be called again.  If
      // |next_delayed_work_time| is null (per Time::is_null), then the queue of
      // future delayed work (timer events) is currently empty, and no additional
      // calls to this function need to be scheduled.
      virtual bool DoDelayedWork(TimeTicks *next_delayed_work_time) = 0;

      // Called from within Run just before the message pump goes to sleep.
      // Returns true to indicate that idle work was done. Returning false means
      // the pump will now wait.
      virtual bool DoIdleWork() = 0;
    };

    virtual void Run(Delegate* delegate) = 0;

    // Quit immediately from the most recently entered run loop.  This method may
    // only be used on the thread that called Run.
    virtual void Quit() = 0;

    // Schedule a DoWork callback to happen reasonably soon.  Does nothing if a
    // DoWork callback is already scheduled.  This method may be called from any
    // thread.  Once this call is made, DoWork should not be "starved" at least
    // until it returns a value of false.
    virtual void ScheduleWork() = 0;

    // Schedule a DoDelayedWork callback to happen at the specified time,
    // cancelling any pending DoDelayedWork callback.  This method may only be
    // used on the thread that called Run.
    virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time) = 0;
  };

}; //base

#endif
