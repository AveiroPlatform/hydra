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

#ifndef MUL_TIME_
#define MUL_TIME_

#ifndef _WIN32
#include "../macros.h"
#endif

namespace base {

  class TimeTicks;

  class Time {
  public:
    static const INT64 kMillisecondsPerSecond = 1000;
    static const INT64 kMicrosecondsPerMillisecond = 1000;
    static const INT64 kMicrosecondsPerSecond = kMicrosecondsPerMillisecond *
      kMillisecondsPerSecond;
    static const INT64 kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
    static const INT64 kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
    static const INT64 kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
    static const INT64 kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
    static const INT64 kNanosecondsPerMicrosecond = 1000;
    static const INT64 kNanosecondsPerSecond = kNanosecondsPerMicrosecond *
      kMicrosecondsPerSecond;
  };

  class TimeDelta {
  public:
    TimeDelta():delta_(0) {}
    explicit TimeDelta(const INT64 &delta):delta_(delta) {}

    TimeDelta& operator=(TimeDelta other) {
      delta_ = other.delta_;
      return *this;
    }

    INT64 ToInternalValue() const {
      return delta_;
    }

    // Returns the time delta in some unit. The F versions return a floating
    // point value, the "regular" versions return a rounded-down value.
    //
    // InMillisecondsRoundedUp() instead returns an integer that is rounded up
    // to the next full millisecond.
    int InDays() const;
    int InHours() const;
    int InMinutes() const;
    double InSecondsF() const;
    INT64 InSeconds() const;
    double InMillisecondsF() const;
    INT64 InMilliseconds() const;
    INT64 InMillisecondsRoundedUp() const;
    INT64 InMicroseconds() const;

    // Converts units of time to TimeDeltas.
    static TimeDelta FromDays(INT64 days);
    static TimeDelta FromHours(INT64 hours);
    static TimeDelta FromMinutes(INT64 minutes);
    static TimeDelta FromSeconds(INT64 secs);
    static TimeDelta FromMilliseconds(INT64 ms);
    static TimeDelta FromMicroseconds(INT64 us);

    // Comparison operators.
    bool operator==(const TimeDelta &other) const {
      return delta_ == other.delta_;
    }

    bool operator!=(const TimeDelta &other) const {
      return delta_ != other.delta_;
    }

    bool operator<(const TimeDelta &other) const {
      return delta_ < other.delta_;
    }

    bool operator<=(const TimeDelta &other) const {
      return delta_ <= other.delta_;
    }

    bool operator>(const TimeDelta &other) const {
      return delta_ > other.delta_;
    }

    bool operator>=(const TimeDelta &other) const {
      return delta_ >= other.delta_;
    }

    // Computations with other deltas.
    TimeDelta operator+(TimeDelta other) const {
      return TimeDelta(delta_ + other.delta_);
    }

    TimeDelta operator-(TimeDelta other) const {
      return TimeDelta(delta_ - other.delta_);
    }

    TimeDelta& operator+=(TimeDelta other) {
      delta_ += other.delta_;
      return *this;
    }

    TimeDelta& operator-=(TimeDelta other) {
      delta_ -= other.delta_;
      return *this;
    }

    TimeDelta operator-() const {
      return TimeDelta(-delta_);
    }

    TimeTicks operator+(TimeTicks t) const;
    // Computations with ints, note that we only allow multiplicative operations
    // with ints, and additive operations with other deltas.
    TimeDelta operator*(INT64 a) const {
      return TimeDelta(delta_ * a);
    }

    TimeDelta operator/(INT64 a) const {
      return TimeDelta(delta_ / a);
    }

    TimeDelta& operator*=(INT64 a) {
      delta_ *= a;
      return *this;
    }

    TimeDelta& operator/=(INT64 a) {
      delta_ /= a;
      return *this;
    }

    INT64 operator/(TimeDelta a) const {
      return delta_ / a.delta_;
    }

    friend class TimeTicks;

  private:
    INT64 delta_;
  };

  // static
  inline TimeDelta TimeDelta::FromDays(INT64 days) {
    return TimeDelta(days * Time::kMicrosecondsPerDay);
  }

  // static
  inline TimeDelta TimeDelta::FromHours(INT64 hours) {
    return TimeDelta(hours * Time::kMicrosecondsPerHour);
  }

  // static
  inline TimeDelta TimeDelta::FromMinutes(INT64 minutes) {
    return TimeDelta(minutes * Time::kMicrosecondsPerMinute);
  }

  // static
  inline TimeDelta TimeDelta::FromSeconds(INT64 secs) {
    return TimeDelta(secs * Time::kMicrosecondsPerSecond);
  }

  // static
  inline TimeDelta TimeDelta::FromMilliseconds(INT64 ms) {
    return TimeDelta(ms * Time::kMicrosecondsPerMillisecond);
  }

  // static
  inline TimeDelta TimeDelta::FromMicroseconds(INT64 us) {
    return TimeDelta(us);
  }

  class TimeTicks {
  public:
    TimeTicks(): ticks_(0) {
    }

    explicit TimeTicks(INT64 ticks): ticks_(ticks) {
    }

    bool is_null() const;

    // Returns the internal numeric value of the TimeTicks object.
    // For serializing, use FromInternalValue to reconstitute.
    INT64 ToInternalValue() const {
      return ticks_;
    }

    //Now
    static TimeTicks Now();

    TimeTicks& operator=(const TimeTicks &other) {
      ticks_ = other.ticks_;
      return *this;
    }

    // Compute the difference between two times.
    TimeDelta operator-(TimeTicks other) const {
      return TimeDelta(ticks_ - other.ticks_);
    }

    // Modify by some time delta.
    TimeTicks& operator+=(TimeDelta delta) {
      ticks_ += delta.delta_;
      return *this;
    }

    TimeTicks& operator-=(TimeDelta delta) {
      ticks_ -= delta.delta_;
      return *this;
    }

    // Return a new TimeTicks modified by some delta.
    TimeTicks operator+(TimeDelta delta) const {
      return TimeTicks(ticks_ + delta.delta_);
    }

    TimeTicks operator-(TimeDelta delta) const {
      return TimeTicks(ticks_ - delta.delta_);
    }

    // Return a new TimeTicks modified by some delta.
    TimeTicks operator+(INT64 delta) const {
      return TimeTicks(ticks_ + delta);
    }

    TimeTicks operator-(INT64 delta) const {
      return TimeTicks(ticks_ - delta);
    }

    // Comparison operators
    bool operator==(const TimeTicks &other) const {
      return ticks_ == other.ticks_;
    }

    bool operator!=(const TimeTicks &other) const {
      return ticks_ != other.ticks_;
    }

    bool operator<(const TimeTicks &other) const {
      return ticks_ < other.ticks_;
    }

    bool operator<=(const TimeTicks &other) const {
      return ticks_ <= other.ticks_;
    }

    bool operator>(const TimeTicks &other) const {
      return ticks_ > other.ticks_;
    }

    bool operator>=(const TimeTicks &other) const {
      return ticks_ >= other.ticks_;
    }

    friend class TimeDelta;

  private:
    INT64 ticks_;
  };

  inline TimeTicks TimeDelta::operator+(TimeTicks t) const {
    return TimeTicks(t.ticks_ + delta_);
  }
} // end base

#endif
