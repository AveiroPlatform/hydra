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
* @file     WrapperObj.h

  @example  

* @author   classfellow@qq.com
* @date     2013-10
* @warning
* @license  BSD 
**           
****************************************************************************************/

#ifndef WRAPPER_OBJ_
#define WRAPPER_OBJ_
//weakprt
//ref count
//Unretained

#include "macros.h"

namespace base {

  template <typename T>
  class  NOVTABLE ObjWrapper {
  public:
    virtual T* get() const = 0;
    virtual void AddRef() const = 0;
    virtual void Release() const = 0;
    virtual bool IsWeakPtr() const = 0;
  };

  template<typename T>
  class NormalWrapper : public ObjWrapper<T> {
  public:
    explicit NormalWrapper(T* o) : ptr_(o) {}
    virtual T *get() const OVERRIDE { return ptr_; }
    virtual void AddRef() const OVERRIDE { ptr_->AddRef(); }
    virtual void Release() const OVERRIDE { ptr_->Release();}
    virtual bool IsWeakPtr() const OVERRIDE { return false; }

  private:
    T * ptr_;
  };

  template <typename T>
  class UnretainedWrapper : public ObjWrapper<T> {
  public:
    explicit UnretainedWrapper(T* o) : ptr_(o) {}
    T* get()         const OVERRIDE { return ptr_; }
    void AddRef()    const OVERRIDE {}
    void Release()   const OVERRIDE {}
    bool IsWeakPtr() const OVERRIDE { return false; }

  private:
    T* ptr_;
  };

  template <typename T>
  static inline UnretainedWrapper<T> *Unretained(T* o) {
    return new UnretainedWrapper<T>(o);
  }

} //end base

#endif
