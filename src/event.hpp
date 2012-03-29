#ifndef _LIBAIO_EVENT_HPP_
#define _LIBAIO_EVENT_HPP_

#include <ev.h>
#include <coroutine++.h>
#include <functional>
#include <stdio.h>

namespace Event {

class ReadEvent {
public:
  ReadEvent(int fd, const std::function<void()> &cb) : callback(cb){
    struct ev_loop *loop = EV_DEFAULT;
    ev_io_init(&event, &ReadEvent::trampoline, fd, EV_READ);
    ev_io_start(EV_A_ &event);
  }

  ~ReadEvent() {
    struct ev_loop *loop = EV_DEFAULT;
    ev_io_stop(EV_A_ &event);
  }

private:
  static void trampoline(EV_P_ ev_io *w, int revents) {
    reinterpret_cast<ReadEvent*>(w)->callback();
  }

  ev_io event;
  std::function<void()> callback;
};

class AcceptEvent {
public:
  AcceptEvent(int fd, const std::function<void()> &cb) : callback(cb){
    struct ev_loop *loop = EV_DEFAULT;
    ev_io_init(&event, &AcceptEvent::trampoline, fd, EV_READ);
    ev_set_priority(&event, 1); // Higher priority than normal
    ev_io_start(EV_A_ &event);
  }

  ~AcceptEvent() {
    struct ev_loop *loop = EV_DEFAULT;
    ev_io_stop(EV_A_ &event);
  }

private:
  static void trampoline(EV_P_ ev_io *w, int revents) {
    reinterpret_cast<AcceptEvent*>(w)->callback();
  }

  ev_io event;
  std::function<void()> callback;
};

class WriteEvent {
public:
  WriteEvent(int fd, const std::function<void()> &cb) : callback(cb){
    struct ev_loop *loop = EV_DEFAULT;
    ev_io_init(&event, &WriteEvent::trampoline, fd, EV_WRITE);
    ev_io_start(EV_A_ &event);
  }

  ~WriteEvent() {
    struct ev_loop *loop = EV_DEFAULT;
    ev_io_stop(EV_A_ &event);
  }

private:
  static void trampoline(EV_P_ ev_io *w, int revents) {
    reinterpret_cast<WriteEvent*>(w)->callback();
  }

  ev_io event;
  std::function<void()> callback;
};

class RunOnceEvent {
public:
  explicit RunOnceEvent(std::function<void()> func) : callback(func) {
    struct ev_loop *loop = EV_DEFAULT;
    ev_idle_init(&event, &RunOnceEvent::trampoline);
    ev_idle_start(EV_A_ &event);
  }

  ~RunOnceEvent() {
    struct ev_loop *loop = EV_DEFAULT;
    ev_idle_stop(EV_A_ &event);
  }

private:
  static void trampoline(EV_P_ ev_idle *w, int revents) {
    RunOnceEvent *this_ = reinterpret_cast<RunOnceEvent*>(w);
    this_->callback();
    delete this_;
  }

  ev_idle event;
  std::function<void()> callback;
};

void RunOnce(std::function<void()> func) {
  new RunOnceEvent(func);
}

void RunLoop() {
  struct ev_loop *loop = EV_DEFAULT;
  ev_run(loop, 0);
}

}

#endif  /* _LIBAIO_EVENT_HPP_ */
