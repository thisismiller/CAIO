#include "caio.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <functional>
#include <queue>
#include "event.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

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
  Event::RunOnce(bind(&Coroutine::run, coro));
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

ssize_t Recv(int fd, void *buf, size_t len, int flags) {
  uint8_t *data = static_cast<uint8_t*>(buf);
  ssize_t added = 0;

  size_t received = added;

  do {
    added = ::recv(fd, data+received, len-received, flags);
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
  } while (received < len);

  return received;
}

ssize_t Send(int fd, const void* buf, size_t len, int flags) {
  const uint8_t *data = static_cast<const uint8_t*>(buf);
  ssize_t wrote = 0;

  size_t written = wrote;

  do {
    wrote = ::send(fd, data+written, len-written, flags);
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
  } while (written < len);

  return written;
}

int Socket(int domain, int type, int protocol) {
  int sock = ::socket(domain, type, protocol);
  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
  return sock;
}

int Connect(int fd, const struct sockaddr *addr, socklen_t addrlen) {
  return ::connect(fd, addr, addrlen);
}

int Close(int fd) {
  return ::close(fd);
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

ssize_t CAIO_Recv(int fd, void *buf, size_t len, int flags) {
  return CAIO::Recv(fd, buf, len, flags);
}

ssize_t CAIO_Send(int fd, const void *buf, size_t len, int flags) {
  return CAIO::Send(fd, buf, len, flags);
}

int CAIO_Socket(int domain, int type, int protocol) {
  return CAIO::Socket(domain, type, protocol);
}

int CAIO_Connect(int fd, const struct sockaddr *addr, socklen_t addrlen) {
  return CAIO::Connect(fd, addr, addrlen);
}

int CAIO_Close(int fd) {
  return CAIO::Close(fd);
}
