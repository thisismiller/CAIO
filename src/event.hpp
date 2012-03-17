#ifndef _LIBAIO_EVENT_HPP_
#define _LIBAIO_EVENT_HPP_

#include <ev++.h>
#include <coroutine++.h>
#include <functional>
#include <stdio.h>

namespace Event {

class ReadEvent {
public:
  ReadEvent(int fd, const std::function<void()> &cb) : callback(cb){
    event.set<ReadEvent,&ReadEvent::trampoline>(const_cast<ReadEvent*>(this));
    event.start(fd, ev::READ);
  }

  ~ReadEvent() {
    event.stop();
  }

private:
  void trampoline(ev::io &w, int revents) {
    callback();
  }

  std::function<void()> callback;
  ev::io event;
};

class WriteEvent {
public:
  WriteEvent(int fd, const std::function<void()> &cb) : callback(cb) {
    event.set<WriteEvent,&WriteEvent::trampoline>(this);
    event.start(fd, ev::WRITE);
  }

  ~WriteEvent() {
    event.stop();
  }

private:
  void trampoline(ev::io &w, int revents) {
    callback();
  }

  std::function<void()> callback;
  ev::io event;
};

class RunOnceEvent {
public:
  RunOnceEvent(std::function<void()> func) : callback(func) {
    event.set<RunOnceEvent, &RunOnceEvent::trampoline>(const_cast<RunOnceEvent*>(this));
    event.start();
  }

  ~RunOnceEvent() {
    event.stop();
  }

private:
  void trampoline(ev::idle &w, int revents) {
    callback();
    delete this;
  }

  std::function<void()> callback;
  ev::idle event;
};

void RunOnce(std::function<void()> func) {
  new RunOnceEvent(func);
}

}

#endif  /* _LIBAIO_EVENT_HPP_ */
