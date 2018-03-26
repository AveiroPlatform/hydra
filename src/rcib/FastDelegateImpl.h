/****************************************************************************************
**
* @file     FastDelegateImpl.h, a implementation for c++ closure
* @brief    supports  function
                      member function
                          weakptr
                          unretained
                          refcount
* @author   Banz
* @mail     classfellow@qq.com
* @date     2013-10
* @updated  2014-10-21 (supports: weakptr, Unretained, refcount)
* @license  Apache-2.0
**
****************************************************************************************/

#ifndef FASTDELEGATEIMPL_
#define FASTDELEGATEIMPL_

#if !defined(_WIN32)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wconversion-null"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#include "FastDelegate.h"
#include "macros.h"

using namespace base;

namespace fastdelegate {

#ifdef FASTDLGT_VC6
#define FASTDLGT_RETTYPE detail::VoidToDefaultVoid<RetType>::type
#else
#define FASTDLGT_RETTYPE RetType
#endif

  class NOVTABLE CCounter{
  public:
    virtual ~CCounter() {};
    virtual unsigned AddRef() = 0;
    virtual unsigned ReleaseRef() = 0;
    static const int IniCounter = 1;
  };

  template<typename RetType>
  class NOVTABLE ExecDelegate: public CCounter{
  public:
    virtual RetType Run() = 0;
    virtual ~ExecDelegate(){}
  };

  // ------**function delegate**--------
  // N=0
  template<class RetType>
  class FunctionEntity0: public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    FunctionEntity0(FASTDLGT_RETTYPE (*func_no_para)()):_Ref(CCounter::IniCounter), m_func_no_para(func_no_para){}
    virtual FASTDLGT_RETTYPE Run(){
      if(m_func_no_para){
        return m_func_no_para();
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    unsigned _Ref;
    FASTDLGT_RETTYPE (*m_func_no_para)();
  };

  // N=1
  template <typename Param1, class P1, class RetType>
  class FunctionEntity1: public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    FunctionEntity1(FASTDLGT_RETTYPE (*func_1_para)(Param1), const P1 &p1):_Ref(CCounter::IniCounter),m_func_1_para(func_1_para), _p1(p1){}
    virtual FASTDLGT_RETTYPE Run(){
      if(m_func_1_para){
        return m_func_1_para(_p1);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    unsigned _Ref;
    FASTDLGT_RETTYPE(*m_func_1_para)(Param1);
    P1 _p1;
  };

  // N=2
  template <typename Param1, class Param2, class P1, class P2, class RetType>
  class FunctionEntity2: public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    FunctionEntity2(FASTDLGT_RETTYPE (*func_2_para)(Param1, Param2), const P1 &p1, const P2 &p2):_Ref(CCounter::IniCounter), m_func_2_para(func_2_para), _p1(p1), _p2(p2){}
    virtual FASTDLGT_RETTYPE Run(){
      if(m_func_2_para){
        return m_func_2_para(_p1, _p2);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    unsigned _Ref;
    FASTDLGT_RETTYPE (*m_func_2_para)(Param1, Param2);
    P1 _p1;
    P2 _p2;
  };

  // N=3
  template <typename Param1, class Param2, class Param3, class P1, class P2, class P3, class RetType>
  class FunctionEntity3 :public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    FunctionEntity3(FASTDLGT_RETTYPE (*func_3_para)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3):_Ref(CCounter::IniCounter), m_func_3_para(func_3_para), _p1(p1), _p2(p2), _p3(p3){}
    virtual FASTDLGT_RETTYPE Run(){
      if(m_func_3_para){
        return m_func_3_para(_p1, _p2, _p3);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    unsigned _Ref;
    FASTDLGT_RETTYPE(*m_func_3_para)(Param1, Param2, Param3);
    P1 _p1;
    P2 _p2;
    P3 _p3;
  };

  // N=4
  template <typename Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4, class RetType>
  class FunctionEntity4 :public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    FunctionEntity4(FASTDLGT_RETTYPE (*func_4_para)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4):_Ref(CCounter::IniCounter), m_func_4_para(func_4_para), _p1(p1), _p2(p2), _p3(p3), _p4(p4){}
    virtual FASTDLGT_RETTYPE Run(){
      if(m_func_4_para){
        return m_func_4_para(_p1, _p2, _p3, _p4);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    unsigned _Ref;
    FASTDLGT_RETTYPE(*m_func_4_para)(Param1, Param2, Param3, Param4);
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
  };

  // N=0
  template <class RetType>
  FunctionEntity0<FASTDLGT_RETTYPE>* MakeFunctionEntity(FASTDLGT_RETTYPE (*func)()){
    return (new FunctionEntity0<FASTDLGT_RETTYPE>(func));
  }

  // N=1
  template <typename Param1, class P1, class RetType>
  inline FunctionEntity1<Param1, P1, FASTDLGT_RETTYPE>* MakeFunctionEntity(FASTDLGT_RETTYPE (*func)(Param1), const P1 &p1){
    return (new FunctionEntity1<Param1, P1, FASTDLGT_RETTYPE>(func, p1));
  }

  // N=2
  template<class Param1, class Param2, class P1, class P2, class RetType>
  inline FunctionEntity2<Param1, Param2, P1, P2, FASTDLGT_RETTYPE>* MakeFunctionEntity(FASTDLGT_RETTYPE (*func)(Param1, Param2), const P1 &p1, const P2 &p2){
    return (new FunctionEntity2<Param1, Param2, P1, P2, FASTDLGT_RETTYPE>(func, p1, p2));
  }

  // N=3
  template <typename Param1, class Param2, class Param3, class P1, class P2, class P3, class RetType>
  inline FunctionEntity3<Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>* MakeFunctionEntity(FASTDLGT_RETTYPE (*func)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3){
    return (new FunctionEntity3<Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>(func, p1, p2, p3));
  }

  // N=4
  template <typename Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4, class RetType>
  inline FunctionEntity4<Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>* MakeFunctionEntity(FASTDLGT_RETTYPE (*func)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4){
    return (FunctionEntity4<Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>(func, p1, p2, p3, p4));
  }

  //------subject delegate-------
  // N=0
  template <class X, class Y, class RetType>
  class DelegateEntity0: public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    DelegateEntity0(Y * that, const FastDelegate0<FASTDLGT_RETTYPE> &func_no_para):m_func_no_para(func_no_para),_Ref(CCounter::IniCounter){
      _that.reset(that);
      if(_that.get()){
        _that->AddRef();
      }
    }
    virtual FASTDLGT_RETTYPE Run(){
      if(_that->IsWeakPtr()){
        if(NULL == _that->get())
          return FASTDLGT_RETTYPE();
      }
      if(m_func_no_para){
        return m_func_no_para();
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        //free memory
        if(_that.get())
          _that->Release();
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    FastDelegate0<FASTDLGT_RETTYPE> m_func_no_para;
    unsigned _Ref;
    std::auto_ptr<Y> _that;
  };

  // N=1
  template <class X, class Y, typename Param1, typename P1, class RetType>
  class DelegateEntity1:public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    DelegateEntity1(Y *that, const FastDelegate1<Param1,FASTDLGT_RETTYPE> &func_1_para, const P1 &p1):m_func_1_para(func_1_para),_Ref(CCounter::IniCounter),_p1(p1){
      _that.reset(that);
      if(_that.get()){
        _that->AddRef();
      }
    }
    virtual FASTDLGT_RETTYPE Run(){
      if(_that->IsWeakPtr()){
        if(NULL == _that->get())
          return FASTDLGT_RETTYPE();
      }
      if(m_func_1_para){
        return m_func_1_para(_p1);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        //free memory
        if(_that.get())
          _that->Release();
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    FastDelegate1<Param1, FASTDLGT_RETTYPE> m_func_1_para;
    unsigned _Ref;
    P1 _p1;
    std::auto_ptr<Y> _that;
  };

  // N=2
  template <class X, class Y, class Param1, class Param2, typename P1, typename P2, class RetType>
  class DelegateEntity2 : public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    DelegateEntity2(Y *that, const FastDelegate2<Param1, Param2, FASTDLGT_RETTYPE> &func_2_para, const P1 &p1, const P2 &p2):m_func_2_para(func_2_para),_Ref(CCounter::IniCounter),_p1(p1),_p2(p2){
      _that.reset(that);
      if(_that.get()){
        _that->AddRef();
      }
    }
    virtual FASTDLGT_RETTYPE Run(){
      if(_that->IsWeakPtr()){
        if(NULL == _that->get())
          return FASTDLGT_RETTYPE();
      }
      if(m_func_2_para){
        return m_func_2_para(_p1,_p2);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        //free memory
        if(_that.get())
          _that->Release();
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    FastDelegate2<Param1, Param2, FASTDLGT_RETTYPE> m_func_2_para;
    unsigned _Ref;
    P1 _p1;
    P2 _p2;
    std::auto_ptr<Y> _that;
  };

  // N=3
  template <class X, class Y, class Param1, class Param2, class Param3, class P1, class P2, typename P3, class RetType>
  class DelegateEntity3 : public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    DelegateEntity3(Y *that, const FastDelegate3<Param1, Param2, Param3, FASTDLGT_RETTYPE> &func_3_para, const P1 &p1, const P2 &p2, const P3 &p3):m_func_3_para(func_3_para),_Ref(CCounter::IniCounter),_p1(p1),_p2(p2),_p3(p3){
      _that.reset(that);
      if(_that.get()){
        _that->AddRef();
      }
    }
    virtual FASTDLGT_RETTYPE Run(){
      if(_that->IsWeakPtr()){
        if(NULL == _that->get())
          return FASTDLGT_RETTYPE();
      }
      if(m_func_3_para){
        return m_func_3_para(_p1, _p2, _p3);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        //free memory
        if(_that.get())
          _that->Release();
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    FastDelegate3<Param1, Param2, Param3, FASTDLGT_RETTYPE> m_func_3_para;
    unsigned _Ref;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    std::auto_ptr<Y> _that;
  };

  // N=4
  template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4, class RetType>
  class DelegateEntity4 : public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    DelegateEntity4(Y* that, const FastDelegate4<Param1, Param2, Param3, Param4, FASTDLGT_RETTYPE> &func_4_para, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4):m_func_4_para(func_4_para), _Ref(CCounter::IniCounter), _p1(p1), _p2(p2), _p3(p3), _p4(p4){
      _that.reset(that);
      if(_that.get()){
        _that->AddRef();
      }
    }
    virtual FASTDLGT_RETTYPE Run(){
      if(_that->IsWeakPtr()){
        if(NULL == _that->get())
          return FASTDLGT_RETTYPE();
      }
      if(m_func_4_para){
        return m_func_4_para(_p1,_p2,_p3,_p4);
      }
      return FASTDLGT_RETTYPE();
    }
    virtual unsigned AddRef(){
      return ++_Ref;
    }
    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        //free memory
        if(_that.get())
          _that->Release();
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    FastDelegate4<Param1, Param2, Param3,Param4, FASTDLGT_RETTYPE> m_func_4_para;
    unsigned _Ref;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
    std::auto_ptr<Y> _that;
  };

  // N=5
  template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class P1, class P2, class P3, class P4, class P5, class RetType>
  class DelegateEntity5 : public ExecDelegate<FASTDLGT_RETTYPE>{
  public:
    DelegateEntity5(Y * that, const FastDelegate5<Param1, Param2, Param3, Param4, Param5, FASTDLGT_RETTYPE> &func_5_para, const P1 &p1,const P2 &p2,const P3 &p3,const P4 &p4,const P5 &p5):m_func_5_para(func_5_para),_Ref(CCounter::IniCounter),_p1(p1),_p2(p2),_p3(p3),_p4(p4),_p5(p5){
      _that.reset(that);
      if(_that.get()){
        _that->AddRef();
      }
    }

    virtual FASTDLGT_RETTYPE Run(){
      if(_that->IsWeakPtr()){
        if(NULL  == _that->get())
          return FASTDLGT_RETTYPE();
      }
      if(m_func_5_para){
        return m_func_5_para(_p1,_p2,_p3,_p4,_p5);
      }
      return FASTDLGT_RETTYPE();
    }

    virtual unsigned AddRef(){
      return ++_Ref;
    }

    virtual unsigned ReleaseRef(){
      --_Ref;
      assert(_Ref >= 0);
      if(0 == _Ref){
        //free memory
        if(_that.get())
          _that->Release();
        delete this;
        return 0;
      }
      return _Ref;
    }

  protected:
    FastDelegate5<Param1, Param2, Param3,Param4, Param5, FASTDLGT_RETTYPE> m_func_5_para;
    unsigned _Ref;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
    P5 _p5;
    std::auto_ptr<Y> _that;
  };

  // Normal
  // N=0
  template <class X, class Y, class RetType>
  inline DelegateEntity0<X, NormalWrapper<Y>, FASTDLGT_RETTYPE>* MakeDelegateEntity(Y* x, RetType (X::*func)()) {
    return (new DelegateEntity0<X, NormalWrapper<Y>, FASTDLGT_RETTYPE>(new NormalWrapper<Y>(x), FastDelegate0<FASTDLGT_RETTYPE>((x), func)));
  }

  // N=1
  template <class X, class Y, class Param1, typename P1, class RetType>
  inline DelegateEntity1<X, NormalWrapper<Y>, Param1, P1, FASTDLGT_RETTYPE>* MakeDelegateEntity(Y* x, RetType (X::*func)(Param1), const P1 &p1){
    return (new DelegateEntity1<X, NormalWrapper<Y>, Param1, P1, FASTDLGT_RETTYPE>(new NormalWrapper<Y>(x), FastDelegate1<Param1, FASTDLGT_RETTYPE>((x), func), p1));
  }

  // N=2
  template<typename X, class Y, class Param1, class Param2, class P1, typename P2, class RetType>
  inline DelegateEntity2<X, NormalWrapper<Y>, Param1, Param2, P1, P2, FASTDLGT_RETTYPE>* MakeDelegateEntity(Y* x, RetType (X::*func)(Param1, Param2), const P1 &p1, const P2 &p2){
    return (new DelegateEntity2<X, NormalWrapper<Y>, Param1, Param2, P1, P2, FASTDLGT_RETTYPE>(new NormalWrapper<Y>(x), FastDelegate2<Param1, Param2, FASTDLGT_RETTYPE>((x), func), p1, p2));
  }

  // N=3
  template <class X, class Y, class Param1, class Param2, class Param3, class P1, typename P2, class P3, class RetType>
  inline DelegateEntity3<X, NormalWrapper<Y>, Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>* MakeDelegateEntity(Y* x, RetType (X::*func)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3){
    return (new DelegateEntity3<X, NormalWrapper<Y>, Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>(new NormalWrapper<Y>(x), FastDelegate3<Param1, Param2, Param3, FASTDLGT_RETTYPE>((x), func), p1, p2 ,p3));
  }

  // N=4
  template<class X, class Y, class Param1, class Param2, class Param3, class Param4, class P1, typename P2, class P3, class P4, class RetType>
  inline DelegateEntity4<X, NormalWrapper<Y>, Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>* MakeDelegateEntity(Y* x, RetType (X::*func)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4){
    return (new DelegateEntity4<X, NormalWrapper<Y>, Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>(new NormalWrapper<Y>(x), FastDelegate4<Param1, Param2, Param3, Param4, FASTDLGT_RETTYPE>((x), func),p1, p2, p3, p4));
  }

  // N=5
  template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class P1, typename P2, class P3, class P4, class P5, class RetType>
  inline DelegateEntity5<X, NormalWrapper<Y>, Param1, Param2, Param3, Param4, Param5, P1, P2, P3, P4, P5, FASTDLGT_RETTYPE>* MakeDelegateEntity(Y* x, RetType (X::*func)(Param1, Param2, Param3, Param4, Param5), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5){
    return (new DelegateEntity5<X, NormalWrapper<Y>, Param1, Param2, Param3, Param4, Param5, P1, P2, P3, P4, P5, FASTDLGT_RETTYPE>(new NormalWrapper<Y>(x), FastDelegate5<Param1, Param2, Param3, Param4, Param5, FASTDLGT_RETTYPE>((x), func), p1, p2, p3, p4, p5));
  }

  // Unretained
  // N=0
  template <class X, class T, class RetType>
  inline DelegateEntity0<X, UnretainedWrapper<T>, FASTDLGT_RETTYPE>* MakeDelegateEntity(UnretainedWrapper<T>* x, RetType (X::*func)()) {
    return (new DelegateEntity0<X, UnretainedWrapper<T>, FASTDLGT_RETTYPE>(x, FastDelegate0<FASTDLGT_RETTYPE>(x->get(), func)));
  }

  // N=1
  template <class X, class T, class Param1, class P1, class RetType>
  inline DelegateEntity1<X, UnretainedWrapper<T>, Param1, P1, FASTDLGT_RETTYPE>* MakeDelegateEntity(UnretainedWrapper<T>* x, RetType (X::*func)(Param1), const P1& p1){
    return (new DelegateEntity1<X, UnretainedWrapper<T>, Param1, P1, FASTDLGT_RETTYPE>(x, FastDelegate1<Param1, FASTDLGT_RETTYPE>(x->get(), func),p1));
  }

  // N=2
  template<typename X, class Y, class Param1, class Param2, class P1, class P2, class RetType>
  inline DelegateEntity2<X, UnretainedWrapper<Y>, Param1, Param2, P1, P2, FASTDLGT_RETTYPE>* MakeDelegateEntity(UnretainedWrapper<Y>* x, RetType (X::*func)(Param1, Param2), const P1 &p1, const P2 &p2){
    return (new DelegateEntity2<X, UnretainedWrapper<Y>, Param1, Param2, P1, P2, FASTDLGT_RETTYPE>(x, FastDelegate2<Param1, Param2, FASTDLGT_RETTYPE>(x->get(), func), p1, p2));
  }

  // N=3
  template <class X, class Y, class Param1, class Param2, class Param3, class P1, class P2, class P3, class RetType>
  inline DelegateEntity3<X, UnretainedWrapper<Y>, Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>* MakeDelegateEntity(UnretainedWrapper<Y>* x, RetType (X::*func)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3){
    return (new DelegateEntity3<X, UnretainedWrapper<Y>, Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>(x, FastDelegate3<Param1, Param2, Param3, FASTDLGT_RETTYPE>(x->get(), func), p1, p2 ,p3));
  }

  // N=4
  template<class X, class Y, class Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4, class RetType>
  inline DelegateEntity4<X, UnretainedWrapper<Y>, Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>* MakeDelegateEntity(UnretainedWrapper<Y>* x, RetType (X::*func)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4){
    return (new DelegateEntity4<X, UnretainedWrapper<Y>, Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>(x, FastDelegate4<Param1, Param2, Param3, Param4, FASTDLGT_RETTYPE>(x->get(), func), p1, p2, p3, p4));
  }

  // N=5
  template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class P1, class P2, class P3, class P4, class P5, class RetType>
  inline DelegateEntity5<X, UnretainedWrapper<Y>, Param1, Param2, Param3, Param4, Param5, P1, P2, P3, P4, P5, FASTDLGT_RETTYPE>* MakeDelegateEntity(UnretainedWrapper<Y>* x, RetType (X::*func)(Param1, Param2, Param3, Param4, Param5), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5){
    return (new DelegateEntity5<X, UnretainedWrapper<Y>, Param1, Param2, Param3, Param4, Param5, P1, P2, P3, P4, P5, FASTDLGT_RETTYPE>(x, FastDelegate5<Param1, Param2, Param3, Param4, Param5, FASTDLGT_RETTYPE>(x->get(), func), p1, p2, p3, p4, p5));
  }

  // WeakPtr
  // N=0
  template <class X, class T, class RetType>
  inline DelegateEntity0<X, WeakPtr<T>, FASTDLGT_RETTYPE>* MakeDelegateEntity(WeakPtr<T>* x, RetType (X::*func)()) {
    return (new DelegateEntity0<X, WeakPtr<T>, FASTDLGT_RETTYPE>(x, FastDelegate0<FASTDLGT_RETTYPE>(x->ptr_, func)));
  }

  // N=1
  template <class X, class T, class Param1, class P1, class RetType>
  inline DelegateEntity1<X, WeakPtr<T>, Param1, P1, FASTDLGT_RETTYPE>* MakeDelegateEntity(WeakPtr<T>* x, RetType (X::*func)(Param1), const P1 &p1){
    return (new DelegateEntity1<X, WeakPtr<T>, Param1, P1, FASTDLGT_RETTYPE>(x, FastDelegate1<Param1, FASTDLGT_RETTYPE>(x->ptr_, func), p1));
  }

  // N=2
  template<typename X, class Y, class Param1, class Param2, class P1, class P2, class RetType>
  inline DelegateEntity2<X, WeakPtr<Y>, Param1, Param2, P1, P2, FASTDLGT_RETTYPE>* MakeDelegateEntity(WeakPtr<Y>* x, RetType (X::*func)(Param1, Param2), const P1 &p1, const P2 &p2){
    return (new DelegateEntity2<X, WeakPtr<Y>, Param1, Param2, P1, P2, FASTDLGT_RETTYPE>(x, FastDelegate2<Param1, Param2, FASTDLGT_RETTYPE>(x->ptr_, func), p1, p2));
  }

  // N=3
  template <class X, class Y, class Param1, class Param2, class Param3, class P1, class P2, class P3, class RetType>
  inline DelegateEntity3<X, WeakPtr<Y>, Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>* MakeDelegateEntity(WeakPtr<Y>* x, RetType (X::*func)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3){
    return (new DelegateEntity3<X, WeakPtr<Y>, Param1, Param2, Param3, P1, P2, P3, FASTDLGT_RETTYPE>(x, FastDelegate3<Param1, Param2, Param3, FASTDLGT_RETTYPE>(x->ptr_, func), p1, p2 ,p3));
  }

  // N=4
  template<class X, class Y, class Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4, class RetType>
  inline DelegateEntity4<X, WeakPtr<Y>, Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>* MakeDelegateEntity(WeakPtr<Y>* x, RetType (X::*func)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4){
    return (new DelegateEntity4<X, WeakPtr<Y>, Param1, Param2, Param3, Param4, P1, P2, P3, P4, FASTDLGT_RETTYPE>(x, FastDelegate4<Param1, Param2, Param3, Param4, FASTDLGT_RETTYPE>(x->ptr_, func), p1, p2, p3, p4));
  }

  // N=5
  template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class P1, class P2, class P3, class P4, class P5, class RetType>
  inline DelegateEntity5<X, WeakPtr<Y>, Param1, Param2, Param3, Param4, Param5, P1, P2, P3, P4, P5, FASTDLGT_RETTYPE>* MakeDelegateEntity(WeakPtr<Y>* x, RetType (X::*func)(Param1, Param2, Param3, Param4, Param5), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5){
    return (new DelegateEntity5<X, WeakPtr<Y>, Param1, Param2, Param3, Param4, Param5, P1, P2, P3, P4, P5, FASTDLGT_RETTYPE>(x, FastDelegate5<Param1, Param2, Param3, Param4, Param5, FASTDLGT_RETTYPE>(x->ptr_, func), p1, p2, p3, p4, p5));
  }

  // a concrete task
  template<typename RetType = void>
  class Task : public base::RefCountedThreadSafe<Task<RetType> >{
  public:
    Task(): m_Func(NULL), m_Class(NULL){
#ifdef _WIN32
      OutputDebugStringA("new TASK\n");
#endif
    }
    ~Task(){
#ifdef _WIN32
      OutputDebugStringA("delete TASK\n");
#endif
      if(m_Func){
        m_Func->ReleaseRef();
      }
      if(m_Class){
        m_Class->ReleaseRef();
      }
    }
    void SetFun(ExecDelegate<FASTDLGT_RETTYPE>* p){ m_Func = p; }
    void SetClass(ExecDelegate<FASTDLGT_RETTYPE>* p){ m_Class = p; }
    FASTDLGT_RETTYPE Run();

  protected:
    ExecDelegate<FASTDLGT_RETTYPE>* m_Func;
    ExecDelegate<FASTDLGT_RETTYPE>* m_Class;
    friend class RefCountedThreadSafe<Task<RetType> >;
  };

  template<class RetType>
  FASTDLGT_RETTYPE Task<FASTDLGT_RETTYPE>::Run(){
    if(m_Class){
      return m_Class->Run();
    }
    if(m_Func){
      return m_Func->Run();
    }
    return FASTDLGT_RETTYPE();
  }

  // clean up after ourselves...
#undef FASTDLGT_RETTYPE
} // end namespace fastdelegate

namespace base {

  inline fastdelegate::Task<void> *Bind(void (*func_no_para)()){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetFun(fastdelegate::MakeFunctionEntity(func_no_para));
    return task;
  }

  template<typename Param1, typename P1>
  inline fastdelegate::Task<void> *Bind(void (*func_1para)(Param1), const P1 &p1){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetFun(fastdelegate::MakeFunctionEntity(func_1para, p1));
    return task;
  }

  template<class Param1, class Param2, typename P1, typename P2>
  inline fastdelegate::Task<void> * Bind(void (*func_2para)(Param1, Param2), const P1 &p1, const P2 &p2){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetFun(fastdelegate::MakeFunctionEntity(func_2para, p1, p2));
    return task;
  }

  template<typename Param1, class Param2, class Param3, class P1, class P2, class P3>
  inline fastdelegate::Task<void> * Bind(void (*func_3para)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetFun(fastdelegate::MakeFunctionEntity(func_3para, p1, p2, p3));
    return task;
  }

  template<typename Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4>
  inline fastdelegate::Task<void> * Bind(void (*func_4para)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetFun(fastdelegate::MakeFunctionEntity(func_4para, p1, p2, p3, p4));
    return task;
  }

  template<class X, class Y>
  inline fastdelegate::Task<void> * Bind(Y *obj, void (X::*func)()){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetClass(fastdelegate::MakeDelegateEntity(obj, func));
    return task;
  }

  template<class X, class Y, class Param1, class P1>
  inline fastdelegate::Task<void> * Bind(Y *obj, void (X::*func)(Param1), const P1 &p1){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetClass(fastdelegate::MakeDelegateEntity(obj, func, p1));
    return task;
  }

  template<class X, class Y, class Param1, class Param2, class P1, class P2>
  inline fastdelegate::Task<void> * Bind(Y *obj, void (X::*func)(Param1, Param2), const P1 &p1, const P2 &p2){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetClass(fastdelegate::MakeDelegateEntity(obj, func, p1, p2));
    return task;
  }

  template<class X, class Y, class Param1, class Param2, class Param3, class P1, class P2, class P3>
  inline fastdelegate::Task<void> * Bind(Y *obj, void (X::*func)(Param1, Param2, Param3), const P1 &p1, const P2 &p2, const P3 &p3){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetClass(fastdelegate::MakeDelegateEntity(obj, func, p1, p2, p3));
    return task;
  }

  template<class X, class Y, class Param1, class Param2, class Param3, class Param4, class P1, class P2, class P3, class P4>
  inline fastdelegate::Task<void> * Bind(Y *obj, void (X::*func)(Param1, Param2, Param3, Param4), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetClass(fastdelegate::MakeDelegateEntity(obj, func, p1, p2, p3, p4));
    return task;
  }

  template<class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class P1, class P2, class P3, class P4, class P5>
  inline fastdelegate::Task<void> * Bind(Y *obj, void (X::*func)(Param1, Param2, Param3, Param4, Param5), const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5){
    fastdelegate::Task<void> *task = new fastdelegate::Task<void>;
    task->SetClass(fastdelegate::MakeDelegateEntity(obj, func, p1, p2, p3, p4, p5));
    return task;
  }

} //end base

#endif
