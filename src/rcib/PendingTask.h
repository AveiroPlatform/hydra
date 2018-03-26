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

#ifndef MUL_PENDING_TASK_
#define MUL_PENDING_TASK_

namespace base {

  class PendingTask {
  public:
    explicit PendingTask();
    explicit PendingTask(fastdelegate::Task<void>* task);
    explicit PendingTask(fastdelegate::Task<void>* task, const TimeTicks delayed_run_time);
    ~PendingTask();

    bool is_null();

    void Reset();

    bool Equals(PendingTask* other) const;

    void Run();
    // Used to support sorting.
    bool operator<(const PendingTask& other) const;
    // The time when the task should be run.
    base::TimeTicks delayed_run_time_;
    // Secondary sort key for run time.
    mutable unsigned int sequence_num_;

  private:
    scoped_refptr<fastdelegate::Task<void> > task_;
  };

  namespace taskqueue {
    class queue {
    public:
      queue():sequence_num_(0) {
        _val.clear();
      }
      ~queue() {
        _val.clear();
      }
      bool empty();
      size_t size();
      void push_back(const PendingTask &task);
      //_val.empty() is false
      PendingTask pick_front();
      PendingTask front();
      void swap(queue &val);

    private:
      std::list<PendingTask> _val;
      unsigned int sequence_num_;
    };
    // PendingTasks are sorted by their |delayed_run_time| property.
  }// end namespace taskqueue

  typedef std::priority_queue<base::PendingTask> DelayedTaskQueue;
  typedef taskqueue::queue TaskQueue;

}// end base

#endif
