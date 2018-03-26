// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

/****************************************************************************************
**
* @file     WeakPtr.h

* @brief    


  @example  

* @author   classfellow@qq.com
* @date     2013-10 
* @updated  2014-10-21
* @warning
* @license  BSD
**           
****************************************************************************************/

#ifndef WEAK_PTR_
#define WEAK_PTR_

namespace base {

  class WeakReference {
  public:
    // Although Flag is bound to a specific thread, it may be deleted from another
    // via base::WeakPtr::~WeakPtr().
    class Flag : public RefCountedThreadSafe<Flag> {
    public:
      Flag(): is_valid_(true) {}

      void Invalidate() {  is_valid_ = false; }
      bool IsValid() const { return is_valid_; }

    private:
      friend class base::RefCountedThreadSafe<Flag>;

      ~Flag();

      bool is_valid_;
    };

    WeakReference();
    explicit WeakReference(const Flag* flag);
    ~WeakReference();

    bool is_valid() const;

  private:
    scoped_refptr<const Flag> flag_;
  };

  class WeakReferenceOwner {
  public:
    WeakReferenceOwner();
    ~WeakReferenceOwner();

    WeakReference GetRef() const;

    bool HasRefs() const {
      return flag_.get() && !flag_->HasOneRef();
    }

    void Invalidate();

  private:
    mutable scoped_refptr<WeakReference::Flag> flag_;
  };

  template<typename T>
  class WeakPtr : public ObjWrapper<T> {
  public:
    WeakPtr() : ptr_(NULL) {
    }
    WeakPtr(WeakReference ref, T* o);
    T* get()         const OVERRIDE { return ref_.is_valid() ? ptr_ : NULL; }
    void AddRef()    const OVERRIDE {}
    void Release()   const OVERRIDE {}
    bool IsWeakPtr() const OVERRIDE;

    T& operator*() const {
      assert(get() != NULL);
      return *get();
    }
    T* operator->() const {
      assert(get() != NULL);
      return get();
    }

    WeakReference ref_;
    T *ptr_;
  };

  template<typename T>
  WeakPtr<T>::WeakPtr(WeakReference ref, T* o):ref_(ref), ptr_(o) {
  }

  template<typename T>
  bool WeakPtr<T>::IsWeakPtr() const {
    return true;
  }

  // A class may extend from SupportsWeakPtr to let others take weak pointers to
  // it. This avoids the class itself implementing boilerplate to dispense weak
  // pointers.  However, since SupportsWeakPtr's destructor won't invalidate
  // weak pointers to the class until after the derived class' members have been
  // destroyed, its use can lead to subtle use-after-destroy issues.
  template <class T>
  class SupportsWeakPtr {
    public:
      SupportsWeakPtr() {}

      //for posttask
      WeakPtr<T> *BldWeakPtr() {
        return new WeakPtr<T>(weak_reference_owner_.GetRef(), static_cast<T*>(this));
      }

      WeakPtr<T> AsWeakPtr() {
        return WeakPtr<T>(weak_reference_owner_.GetRef(), static_cast<T*>(this));
      }

    protected:
      ~SupportsWeakPtr() {}

    private:
      WeakReferenceOwner weak_reference_owner_;
      DISALLOW_COPY_AND_ASSIGN_(SupportsWeakPtr);
  };

} //end namespace base

#endif
