// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include "../macros.h"
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "../util_tools.h"
#else
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#endif
#include "time.h"

namespace base {

#ifdef _WIN32
  // Accumulation of time lost due to rollover (in milliseconds).
  INT64 rollover_ms = 0;

  // The last timeGetTime value we saw, to detect rollover.
  DWORD last_seen_now = 0;

  CriticalSection m_cs;

  TimeDelta RolloverProtectedNow() {
    AutoCritSecLock<CriticalSection> lock(m_cs, false);
    lock.Lock();
    // We should hold the lock while calling tick_function to make sure that
    // we keep last_seen_now stay correctly in sync.
    DWORD now = timeGetTime();
    if (now < last_seen_now)
      rollover_ms += 0x100000000I64;  // ~49.7 days.
    last_seen_now = now;
    lock.Unlock();
    return TimeDelta::FromMilliseconds(now + rollover_ms);
  }
#endif

  int TimeDelta::InDays() const {
    return static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
  }

  int TimeDelta::InHours() const {
    return static_cast<int>(delta_ / Time::kMicrosecondsPerHour);
  }

  int TimeDelta::InMinutes() const {
    return static_cast<int>(delta_ / Time::kMicrosecondsPerMinute);
  }

  double TimeDelta::InSecondsF() const {
    return static_cast<double>(delta_) / Time::kMicrosecondsPerSecond;
  }

  INT64 TimeDelta::InSeconds() const {
    return delta_ / Time::kMicrosecondsPerSecond;
  }

  double TimeDelta::InMillisecondsF() const {
    return static_cast<double>(delta_) / Time::kMicrosecondsPerMillisecond;
  }

  INT64 TimeDelta::InMilliseconds() const {
    return delta_ / Time::kMicrosecondsPerMillisecond;
  }

  INT64 TimeDelta::InMillisecondsRoundedUp() const {
    return (delta_ + Time::kMicrosecondsPerMillisecond - 1) /
      Time::kMicrosecondsPerMillisecond;
  }

  INT64 TimeDelta::InMicroseconds() const {
    return delta_;
  }

  bool TimeTicks::is_null() const {
    return 0 == ticks_;
  }

  // static
  TimeTicks TimeTicks::Now() {
#ifdef _WIN32
    return TimeTicks() + RolloverProtectedNow();
#else
    UINT64 absolute_micro;

    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
      assert(0);
      return TimeTicks();
    }

    absolute_micro =
      (static_cast<INT64>(ts.tv_sec) * Time::kMicrosecondsPerSecond) +
      (static_cast<INT64>(ts.tv_nsec) / Time::kNanosecondsPerMicrosecond);

    return TimeTicks(absolute_micro);
#endif
  }
} // end base
