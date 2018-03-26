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

#ifdef _WIN32
#include <windows.h>
#endif
#include <string>
#include <memory>
#include <stack>
#include <assert.h>
#include "macros.h"
#include "ref_counted.h"
#include "WrapperObj.h"
#include "WeakPtr.h"
#include "FastDelegateImpl.h"
#include "util_tools.h"
#include "at_exist.h"

extern base::AtExitManager* g_top_manager;

namespace base {

  AtExitManager::AtExitManager() {
    g_top_manager = this;
  }

  AtExitManager::~AtExitManager() {
    assert(this == g_top_manager);
    ProcessCallbacksNow();
    g_top_manager = NULL;
  }

  // static
  void AtExitManager::RegisterCallback(AtExitCallbackType func, void* param) {
    assert(func);
    RegisterTask(base::Bind(func, param));
  }

  // static
  void AtExitManager::RegisterTask(fastdelegate::Task<void>* task) {
    if (!g_top_manager) {
      assert(0); // "Tried to RegisterCallback without an AtExitManager";
      return;
    }

    AutoCritSecLock<CriticalSection> lock(g_top_manager->m_cs, false);
    lock.Lock();
    g_top_manager->stack_.push(task);
  }

  // static
  void AtExitManager::ProcessCallbacksNow() {
    if (!g_top_manager) {
      assert(0); //"Tried to ProcessCallbacksNow without an AtExitManager";
      return;
    }

    AutoCritSecLock<CriticalSection> lock(g_top_manager->m_cs, false);
    lock.Lock();

    while (!g_top_manager->stack_.empty()) {
      std::auto_ptr<fastdelegate::Task<void> > task(g_top_manager->stack_.top());
      task->Run();
      g_top_manager->stack_.pop();
    }
  }
}
