#include "aio.h"

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
  AIO::Yield();
}
}

namespace AIO {

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

}  /* AIO */

void AIO_Spawn(void (*func)(void*), void* context) {
  AIO::Spawn(std::bind(func,context));
}

void AIO_Yield() {
  AIO::Yield();
}

void AIO_RunIOLoop() {
  AIO::RunIOLoop();
}

ssize_t AIO_Read(int fd, void* buf, size_t count) {
  return AIO::Read(fd, buf, count);
}

ssize_t AIO_Write(int fd, const void *buf, size_t count) {
  return AIO::Write(fd, buf, count);
}
