// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef PACMAN_CLIENT_SRC_MUTEX_DEQUE_H_
#define PACMAN_CLIENT_SRC_MUTEX_DEQUE_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <deque>


namespace fun {
namespace util {

/// MutexDeque 는 여러 쓰레드가 동시에 액세스할 때
/// 뮤텍스를 이용하여 순서대로 액세스하는 컨테이너이다.
/// 내부 자료구조는 std::deque 이다.
/// @see MutexDeque
template <typename T>
class MutexDeque {
 public:
  typedef std::deque<T> BaseDeque;

  MutexDeque();
  void Push(const T &t);
  const T &Front();
  void Pop();
  bool Contains() const;
  boost::shared_ptr<BaseDeque> CreateInfo() const;  // for debugging.

 private:
  mutable boost::mutex mutex_;
  BaseDeque deque_;
};


template <typename T>
MutexDeque<T>::MutexDeque() : mutex_(), deque_() {
}


template <typename T>
void MutexDeque<T>::Push(const T &t) {
  boost::mutex::scoped_lock lock(mutex_);
  deque_.push_back(t);
}


template <typename T>
const T &MutexDeque<T>::Front() {
  boost::mutex::scoped_lock lock(mutex_);
  return deque_.front();
}


template <typename T>
void MutexDeque<T>::Pop() {
  boost::mutex::scoped_lock lock(mutex_);
  deque_.pop_front();
}


template <typename T>
bool MutexDeque<T>::Contains() const {
  boost::mutex::scoped_lock lock(mutex_);
  return !deque_.empty();
}


template <typename T>
boost::shared_ptr<typename MutexDeque<T>::BaseDeque>
MutexDeque<T>::CreateInfo() const {
  boost::shared_ptr<BaseDeque> clone(new BaseDeque());
  {
    boost::mutex::scoped_lock lock(mutex_);
    *clone = deque_;
  }
  return clone;
}

}  // namespace util
}  // namespace fun

#endif  // PACMAN_CLIENT_SRC_MUTEX_DEQUE_H_
