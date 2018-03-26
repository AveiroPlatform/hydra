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

#if !defined(_WIN32)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#ifdef _WIN32
#include <windows.h>
#include <assert.h>
#include "time/time.h"
#else
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#include "util_tools.h"

CriticalSection::CriticalSection(void) {
#ifdef _WIN32
	InitializeCriticalSection(&m_cs);
#else
  pthread_mutex_init(&m_cs, NULL);
#endif
}

CriticalSection::~CriticalSection(void) {
#ifdef _WIN32
	DeleteCriticalSection(&m_cs);
#else
  pthread_mutex_destroy(&m_cs);
#endif
}

void CriticalSection::Lock(void) {
#ifdef _WIN32
  EnterCriticalSection(&m_cs);
#else
  pthread_mutex_lock(&m_cs);
#endif
}

void CriticalSection::UnLock(void) {
#ifdef _WIN32
  LeaveCriticalSection(&m_cs);
#else
  pthread_mutex_unlock(&m_cs);
#endif
}

bool CriticalSection::TryLock(void) {
	bool bReturn = false;

#ifdef _WIN32
  bReturn = (bool)TryEnterCriticalSection(&m_cs);
#else
  bReturn = pthread_mutex_trylock(&m_cs);
#endif

	return bReturn;
}

#ifdef _WIN32
CRITICAL_SECTION * CriticalSection::os_lock() {
#else
pthread_mutex_t* CriticalSection::os_lock() {
#endif
  return &m_cs;
}

#ifndef _WIN32

ConditionVariable::ConditionVariable(CriticalSection * user_lock):
  user_mutex_(user_lock->os_lock()) {
  int rv = pthread_cond_init(&condition_, NULL);
  DCHECK_EQ(0, rv);
}

ConditionVariable::~ConditionVariable() {
  int rv = pthread_cond_destroy(&condition_);
  DCHECK_EQ(0, rv);
}

void ConditionVariable::Wait() {
  int rv = pthread_cond_wait(&condition_, user_mutex_);
  DCHECK_EQ(0, rv);
}

void ConditionVariable::TimedWait(const base::TimeDelta& max_time) {

  INT64 usecs = max_time.InMicroseconds();

  // The timeout argument to pthread_cond_timedwait is in absolute time.
  struct timeval now;
  gettimeofday(&now, NULL);

  struct timespec abstime;
  abstime.tv_sec = now.tv_sec + (usecs / base::Time::kMicrosecondsPerSecond);
  abstime.tv_nsec = (now.tv_usec + (usecs % base::Time::kMicrosecondsPerSecond)) *
       base::Time::kNanosecondsPerMicrosecond;
  abstime.tv_sec += abstime.tv_nsec / base::Time::kNanosecondsPerSecond;
  abstime.tv_nsec %= base::Time::kNanosecondsPerSecond;
  DCHECK_GE(abstime.tv_sec, now.tv_sec);  // Overflow paranoia

  int rv = pthread_cond_timedwait(&condition_, user_mutex_, &abstime);
  assert(rv == 0 || rv == ETIMEDOUT);
}

void ConditionVariable::Signal() {
  int rv = pthread_cond_signal(&condition_);
  DCHECK_EQ(0, rv);
}

void ConditionVariable::Broadcast() {
  int rv = pthread_cond_broadcast(&condition_);
  DCHECK_EQ(0, rv);
}

#endif
