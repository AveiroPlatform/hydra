#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <assert.h>
#endif
#include "macros.h"
#include "time/time.h"
#include "MessagePump.h"
#include "Event/WaitableEvent.h"
#include "MessagePumpDefault.h"

namespace base {

  MessagePumpDefault::MessagePumpDefault(): keep_running_(true), event_(false, false) {

  }

  MessagePumpDefault::~MessagePumpDefault() {

  }

  void MessagePumpDefault::Run(Delegate *delegate) {
    for (;;) {
      bool did_work = delegate->DoWork();
      if (false == keep_running_)
        break;

      did_work |= delegate->DoDelayedWork(&delayed_work_time_);
      if (false == keep_running_)
        break;

      if (did_work)
        continue;

      did_work = delegate->DoIdleWork();
      if (false == keep_running_)
        break;

      if (did_work)
        continue;

      if (delayed_work_time_.is_null()) {
        event_.Wait();
      } else {
        TimeDelta delay = delayed_work_time_ - TimeTicks::Now();
        if (delay > TimeDelta()) {
          event_.TimedWait(delay);
        } else {
          // It looks like delayed_work_time_ indicates a time in the past, so we
          // need to call DoDelayedWork now.
          delayed_work_time_ = TimeTicks();
        }
      }
    }
    keep_running_ = true;
  }

  void MessagePumpDefault::Quit() {
    keep_running_ = false;
  }

  void MessagePumpDefault::ScheduleWork() {
    event_.Signal();
  }

  void MessagePumpDefault::ScheduleDelayedWork(
    const TimeTicks& delayed_work_time) {
      // We know that we can't be blocked on Wait right now since this method can
      // only be called on the same thread as Run, so we only need to update our
      // record of how long to sleep when we do sleep.
      delayed_work_time_ = delayed_work_time;
  }

} //end base
