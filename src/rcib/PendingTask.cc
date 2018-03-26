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

#ifdef _WIN32
#include <windows.h>
#else
#include <assert.h>
#endif
#include <string>
#include <memory>
#include <list>
#include "macros.h"
#include "ref_counted.h"
#include "WrapperObj.h"
#include "WeakPtr.h"
#include "FastDelegateImpl.h"
#include "time/time.h"
#include <queue>
#include "PendingTask.h"

namespace base {

  PendingTask::PendingTask():task_(NULL), sequence_num_(0) {}

  PendingTask::PendingTask(fastdelegate::Task<void>* task):task_(task),sequence_num_(0) {}

  PendingTask::PendingTask(fastdelegate::Task<void>* task, const TimeTicks delayed_run_time)
    :task_(task), delayed_run_time_(delayed_run_time), sequence_num_(0) {}

  PendingTask::~PendingTask() {}

  bool PendingTask::is_null() {
    return NULL == task_.get();
  }

  void PendingTask::Reset() {
    task_ = NULL;
  }

  bool PendingTask::Equals(PendingTask* other) const {
    return task_.get() == other->task_.get();
  }

  void PendingTask::Run() {
    if(task_.get()) {
      task_->Run();
    }
  }

  bool PendingTask::operator<(const PendingTask& other) const {
    // Since the top of a priority queue is defined as the "greatest" element, we
    // need to invert the comparison here.  We want the smaller time to be at the
    // top of the heap.

    if (delayed_run_time_ < other.delayed_run_time_)
      return false;

    if (delayed_run_time_ > other.delayed_run_time_)
      return true;

    // If the times happen to match, then we use the sequence number to decide.
    // Compare the difference to support integer roll-over.
    return (sequence_num_ - other.sequence_num_) > 0;
  }
  //task queue
  namespace taskqueue {
    bool queue::empty() {
      return _val.empty();
    }

    size_t queue::size() {
      return _val.size();
    }

    void queue::push_back(const PendingTask &val) {
      val.sequence_num_ = ++sequence_num_;
      _val.push_back(val);
    }

    PendingTask queue::pick_front() {
      assert(_val.size());
      PendingTask o = _val.front();
      _val.pop_front();
      return o;
    }

    PendingTask queue::front() {
      assert(_val.size());
      return _val.front();
    }

    void queue::swap(taskqueue::queue &val) {
      _val.swap(val._val);
    }
  }

} // end
