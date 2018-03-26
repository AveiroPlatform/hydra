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

#ifndef MUL_UTIL_TOOL_
#define MUL_UTIL_TOOL_

#include "macros.h"
#ifndef _WIN32
#include <pthread.h>
#include <assert.h>
#include "time/time.h"
#endif
#include <string>

class CriticalSection {
public:
	CriticalSection(void);
	~CriticalSection(void);

	void Lock(void);
	void UnLock(void);
	bool TryLock(void);

#ifdef _WIN32
  CRITICAL_SECTION * os_lock();
#else
  pthread_mutex_t * os_lock();
#endif

private:
#ifdef _WIN32
  CRITICAL_SECTION  m_cs;
#else
  pthread_mutex_t  m_cs;
#endif
};

#ifndef _WIN32
class ConditionVariable {
public:
  // Construct a cv for use with ONLY one user lock.
  explicit ConditionVariable(CriticalSection* user_lock);
  ~ConditionVariable();
  // Wait() releases the caller's critical section atomically as it starts to
  // sleep, and the reacquires it when it is signaled.
  void Wait();
  void TimedWait(const base::TimeDelta& max_time);

  // Signal() revives one waiting thread.
  void Signal();
  // Broadcast() revives all waiting threads.
  void Broadcast();

private:
  pthread_cond_t condition_;
  pthread_mutex_t* user_mutex_;
};
#endif

template<typename TLock>
class AutoCritSecLock {
public:
  AutoCritSecLock( TLock& cs, bool bInitialLock = true );
  ~AutoCritSecLock() throw();

  bool Lock() throw();
  void Unlock() throw();

  // Implementation
private:
  TLock& m_cs;
  bool m_bLocked;

  // Private to avoid accidental use
  AutoCritSecLock( const AutoCritSecLock& ) throw();
  AutoCritSecLock& operator=( const AutoCritSecLock& ) throw();
};

template< class TLock >
inline AutoCritSecLock< TLock >::AutoCritSecLock( TLock& cs, bool bInitialLock ) : m_cs( cs ), m_bLocked( false ) {
  if( bInitialLock ) {
    Lock();
  }
}

template< class TLock >
inline AutoCritSecLock< TLock >::~AutoCritSecLock() throw() {
  if( m_bLocked ) {
    Unlock();
  }
}

template< class TLock >
inline bool AutoCritSecLock< TLock >::Lock() throw() {
  m_cs.Lock();
  m_bLocked = true;
  return( true );
}

template< class TLock >
inline void AutoCritSecLock< TLock >::Unlock() throw() {
  m_cs.UnLock();
  m_bLocked = false;
}

#endif
