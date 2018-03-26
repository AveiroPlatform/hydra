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

#ifndef THREAD_LOCAL_
#define THREAD_LOCAL_

#ifndef _WIN32
#include <pthread.h>
#endif

namespace base {
  template <typename Type>
  class ThreadLocalPointer {
  public:
    ThreadLocalPointer() {
#ifdef _WIN32
      slot_ = TlsAlloc();
      assert(slot_ != TLS_OUT_OF_INDEXES);
#else
      int error = pthread_key_create(&slot_, NULL);
      DCHECK_EQ(error, 0);
#endif
    }

    ~ThreadLocalPointer() {
#ifdef _WIN32
      if(!TlsFree(slot_)) {
        assert(0);
      }
#else
      int error = pthread_key_delete(slot_);
      DCHECK_EQ(0, error);
#endif
    }

    Type* Get() {
#ifdef _WIN32
      return static_cast<Type*>(
        TlsGetValue(slot_));
#else
      return static_cast<Type*>(
        pthread_getspecific(slot_));
#endif
    }

    void Set(Type* ptr) {
#ifdef _WIN32
      if(!TlsSetValue(slot_, const_cast<void*>(static_cast<const void*>(ptr)))) {
        assert(0);
      }
#else
      int error = pthread_setspecific(slot_, const_cast<void*>(static_cast<const void*>(ptr)));
      DCHECK_EQ(error, 0);
#endif
    }

  private:
#ifdef _WIN32
    DWORD slot_;
#else
    pthread_key_t slot_;
#endif
    DISALLOW_COPY_AND_ASSIGN_(ThreadLocalPointer<Type>);
  };
}

#endif
