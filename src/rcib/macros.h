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

#ifndef MUL_MACROS_
#define MUL_MACROS_

#ifdef _WIN32
#define OVERRIDE override
#define NOVTABLE  __declspec(novtable)
#endif

#ifndef _WIN32
#include <stddef.h>
typedef long long  INT64;
typedef unsigned long long  UINT64;
typedef int BOOL;
#define OVERRIDE
#define NOVTABLE
#define S_OK 0
typedef long intptr_t;
#endif

#define DISALLOW_COPY_AND_ASSIGN_(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                    \
  DISALLOW_COPY_AND_ASSIGN_(TypeName)

#define TASK  fastdelegate::Task<void>
#define build new

#ifdef _WIN32
#define ALIGNAS(byte_alignment)  __declspec(align(byte_alignment))
#else
#define ALIGNAS(byte_alignment)  __attribute__((aligned(byte_alignment)))
#endif

#define MAKETASK_FUN0(task, fun) \
  (task->SetFun(fastdelegate::MakeFunctionEntity(fun)) , task)

#define MAKETASK_FUN1(task, fun, p1) \
  (task->SetFun(fastdelegate::MakeFunctionEntity(fun, (p1))) , task)

#define MAKETASK_FUN2(task, fun, p1, p2) \
  (task->SetFun(fastdelegate::MakeFunctionEntity(fun, (p1), (p2))) , task)

#define MAKETASK_FUN3(task, fun, p1, p2, p3) \
  (task->SetFun(fastdelegate::MakeFunctionEntity(fun, (p1), (p2), (p3))) , task)

#define MAKETASK_FUN4(task, fun, p1, p2, p3, p4) \
  (task->SetFun(fastdelegate::MakeFunctionEntity(fun, (p1), (p2), (p3), (p4))) , task)

#define MAKETASK_CLOSURE0(task, obj, fun) \
  (task->SetClass(fastdelegate::MakeDelegateEntity(&(obj), &fun)), task)

#define MAKETASK_CLOSURE1(task, obj, fun, p1) \
  (task->SetClass(fastdelegate::MakeDelegateEntity(&(obj), &fun, (p1))), task)

#define MAKETASK_CLOSURE2(task, obj, fun, p1, p2) \
  (task->SetClass(fastdelegate::MakeDelegateEntity(&(obj), &fun, (p1), (p2))), task)

#define MAKETASK_CLOSURE3(task, obj, fun, p1, p2, p3) \
  (task->SetClass(fastdelegate::MakeDelegateEntity(&(obj), &fun, (p1), (p2), (p3))), task)

#define MAKETASK_CLOSURE4(task, obj, fun, p1, p2, p3, p4) \
  (task->SetClass(fastdelegate::MakeDelegateEntity(&(obj), &fun, (p1), (p2), (p3), (p4))), task)

#define MAKETASK_CLOSURE5(task, obj, fun, p1, p2, p3, p4, p5) \
  (task->SetClass(fastdelegate::MakeDelegateEntity(&(obj), &fun, (p1), (p2), (p3), (p4), (p5))), task)

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

//// It's possible for functions that use a va_list, such as StringPrintf, to
//// invalidate the data in it upon use.  The fix is to make a copy of the
//// structure before using it and use that copy instead.  va_copy is provided
//// for this purpose.  MSVC does not provide va_copy, so define an
//// implementation here.  It is not guaranteed that assignment is a copy, so the
//// StringUtil.VariableArgsFunc unit test tests this capability.
//#if defined(COMPILER_GCC)
//#define GG_VA_COPY(a, b) (va_copy(a, b))
//#elif defined(COMPILER_MSVC)
//#define GG_VA_COPY(a, b) (a = b)
//#endif

#define DCHECK(v) assert((v));
#define DCHECK_GE(a, b) assert((a) >= (b));
#define DCHECK_LE(a, b) assert((a) <= (b));
#define DCHECK_NE(a, b) assert((a) != (b));
#define DCHECK_EQ(a, b) assert((a) == (b));
#define DCHECK_G(a, b) assert((a) > (b));
#define DCHECK_L(a, b) assert((a) < (b));

#endif
