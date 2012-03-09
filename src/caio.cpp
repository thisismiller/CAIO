#include "caio.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <functional>
#include <queue>
#include "event.hpp"

namespace {
void DeleteMeAtExit(std::function<void()> func) {
  func();
  delete &Coroutine::Running();
  CAIO::Yield();
}
}

namespace CAIO {

void Spawn(std::function<void()> func) {
  Coroutine::SetScheduler(Coroutine::Main());
  Coroutine *coro = new Coroutine(bind(DeleteMeAtExit, func)); 
  coro->run();
}

void Yield() {
  Coroutine::Main().run();
}

void RunIOLoop() {
  ev::get_default_loop().run();
}

ssize_t Read(int fd, void* buf, size_t count) {
  uint8_t *data = static_cast<uint8_t*>(buf);
  ssize_t added = 0;

  size_t received = added;

  do {
    added = ::read(fd, data+received, count-received);
    if (added > 0) {
      received += added;
    } else if (added == 0) {
      return received;
    } else if (added == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::ReadEvent event(fd, bind(&Coroutine::run, Coroutine::Running()));
      Yield();
    }
  } while (received < count);

  return received;
}

ssize_t Write(int fd, const void *buf, size_t count) {
  const uint8_t *data = static_cast<const uint8_t*>(buf);
  ssize_t wrote = 0;

  size_t written = wrote;

  do {
    wrote = ::write(fd, data+written, count-written);
    if (wrote > 0) {
      written += wrote;
    } else if (wrote == 0) {
      return written;
    } else if (wrote == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::WriteEvent event(fd, bind(&Coroutine::run, Coroutine::Running()));
      Yield();
    }
  } while (written < count);

  return written;
}

}  /* CAIO */

void CAIO_Spawn(void (*func)(void*), void* context) {
  CAIO::Spawn(std::bind(func,context));
}

void CAIO_Yield() {
  CAIO::Yield();
}

void CAIO_RunIOLoop() {
  CAIO::RunIOLoop();
}

ssize_t CAIO_Read(int fd, void* buf, size_t count) {
  return CAIO::Read(fd, buf, count);
}

ssize_t CAIO_Write(int fd, const void *buf, size_t count) {
  return CAIO::Write(fd, buf, count);
}
