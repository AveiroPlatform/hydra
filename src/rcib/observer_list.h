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

#ifndef MUL_OBSERVER_LIST_
#define MUL_OBSERVER_LIST_

#include <algorithm>

namespace base {

  template <typename T> class SupportsWeakPtr;
  template <typename T> class WeakPtr;

  template <class ObserverType>
  class ObserverListBase:
    public base::SupportsWeakPtr<ObserverListBase<ObserverType> > {
  public:
    ObserverListBase()
      :remov_count_(0){}

    // An iterator class that can be used to access the list of observers.  See
    // also the FOR_EACH_OBSERVER macro defined below.
    class Iterator {
    public:
      Iterator(ObserverListBase<ObserverType>& list):index_(0)
      {
        list_ = list.AsWeakPtr();
      }

      ~Iterator() {
        if (list_.get())
          list_->Compact();          
      }

      ObserverType* GetNext() {
        if (!list_.get())
          return NULL;
        ListType& observers = list_->observers_;
        // Advance if the current element is null
        size_t max_index = observers.size();
        while (index_ < max_index && !observers[index_])
          ++index_;
        return index_ < max_index ? observers[index_++] : NULL;
      }

    private:
      base::WeakPtr<ObserverListBase<ObserverType> > list_;
      size_t index_;
    };
    // Add an observer to the list.  An observer should not be added to
    // the same list more than once.
    void AddObserver(ObserverType* obs) {
      if (std::find(observers_.begin(), observers_.end(), obs)
        != observers_.end()) {
          assert(0);
          return;
      }
      observers_.push_back(obs);
    }

    // Remove an observer from the list if it is in the list.
    void RemoveObserver(ObserverType* obs) {
      typename ListType::iterator it =
        std::find(observers_.begin(), observers_.end(), obs);
      if (it != observers_.end()) {
        *it = NULL;
      }
      ++remov_count_;
      if(remov_count_ > 128){
        remov_count_ = 0;
        Compact();
      }
    }

    bool HasObserver(ObserverType* observer) const {
      for (size_t i = 0; i < observers_.size(); ++i) {
        if (observers_[i] == observer)
          return true;
      }
      return false;
    }

    void Clear() {
      observers_.clear();
    }

    size_t size() const { return observers_.size(); }

  protected:
    void Compact() {
      observers_.erase(
        std::remove(observers_.begin(), observers_.end(),
        static_cast<ObserverType*>(NULL)), observers_.end());
    }

  private:
    typedef std::vector<ObserverType*> ListType;

    ListType observers_;

    int remov_count_;

    friend class ObserverListBase::Iterator;
    DISALLOW_COPY_AND_ASSIGN_(ObserverListBase);
  };

  template <class ObserverType, bool check_empty = false>
  class ObserverList : public ObserverListBase<ObserverType> {
  public:
    ObserverList() {}

    ~ObserverList() {
      // When check_empty is true, assert that the list is empty on destruction.
      if (check_empty) {
        ObserverListBase<ObserverType>::Compact();
      }
    }

    bool might_have_observers() const {
      return ObserverListBase<ObserverType>::size() != 0;
    }
  };

#define FOR_EACH_OBSERVER(ObserverType, observer_list, func)          \
  do {                                                                \
    if ((observer_list).might_have_observers()) {                     \
      ObserverListBase<ObserverType>::Iterator                        \
      it_inside_observer_macro(observer_list);                        \
      ObserverType* obs;                                              \
      while ((obs = it_inside_observer_macro.GetNext()) != NULL)      \
        obs->func;                                                    \
    }                                                                 \
  } while (0)

} //end base

#endif
