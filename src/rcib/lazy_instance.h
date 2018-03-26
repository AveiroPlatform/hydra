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

#ifndef LAZY_INSTANCE_
#define LAZY_INSTANCE_

#include "macros.h"

#define LAZY_INSTANCE_INITIALIZER {0}

namespace base {

  void CompleteLazyInstance(intptr_t* state,
    intptr_t new_instance,
    void* lazy_instance,
    void (*dtor)(void*));

  template <typename Type>
  struct DefaultLazyInstanceTraits {
    static Type* New(void* instance) {
      return new (instance) Type();
    }

    static void Delete(Type* instance) {
      // Explicitly call the destructor.
      instance->~Type();
    }
  };

  template <typename Type, typename Traits = DefaultLazyInstanceTraits<Type> >
  class LazyInstance {
  public:
    // Do not define a destructor, as doing so makes LazyInstance a
    // non-POD-struct. We don't want that because then a static initializer will
    // be created to register the (empty) destructor with atexit() under MSVC, for
    // example. We handle destruction of the contained Type class explicitly via
    // the OnExit member function, where needed.
    // ~LazyInstance() {}


    Type& Get() {
      return *Pointer();
    }

    Type* Pointer() {

      // We will hopefully have fast access when the instance is already created.
      // Since a thread sees private_instance_ == 0 or kLazyInstanceStateCreating
      // at most once, the load is taken out of NeedsInstance() as a fast-path.
      // The load has acquire memory ordering as a thread which sees
      // private_instance_ > creating needs to acquire visibility over
      // the associated data (private_buf_). Pairing Release_Store is in
      // CompleteLazyInstance().
      intptr_t value = private_instance_;
      if (!value) {
        // Create the instance in the space provided by |private_buf_|.
        value = reinterpret_cast<intptr_t>(
          Traits::New(private_buf_.void_data()));
        CompleteLazyInstance(&private_instance_, value, this, OnExit);
      }

      return instance();
    }

    bool operator==(Type* p) {
      switch(private_instance_){
      case 0:
        return p == NULL;
      default:
        return p == instance();
      }
    }

    // Effectively private: member data is only public to allow the linker to
    // statically initialize it and to maintain a POD class. DO NOT USE FROM
    // OUTSIDE THIS CLASS.

    intptr_t private_instance_;
    // Preallocated space for the Type instance.
    base::AlignedMemory<sizeof(Type), __alignof(Type)> private_buf_;

  private:
    Type* instance() {
      return reinterpret_cast<Type*>(private_instance_);
    }

    // Adapter function for use with AtExit.  This should be called single
    // threaded, so don't synchronize across threads.
    // Calling OnExit while the instance is in use by other threads is a mistake.
    static void OnExit(void* lazy_instance) {
      LazyInstance<Type, Traits>* me =
        reinterpret_cast<LazyInstance<Type, Traits>*>(lazy_instance);
      Traits::Delete(me->instance());
      me->private_instance_ = 0;
    }
  };
}

#endif
