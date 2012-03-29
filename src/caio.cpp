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
void DeleteCoroutine(Coroutine *coro) {
  delete coro;
}

void DeleteMeAtExit(std::function<void()> func) {
  func();
  Event::RunOnce(bind(DeleteCoroutine, &Coroutine::Running()));
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

void SetStackSize(size_t size) {
  Coroutine::SetDefaultStackSize(size);
}

ssize_t Read(int fd, void* buf, size_t count) {
  uint8_t *data = static_cast<uint8_t*>(buf);
  ssize_t received = 0;

  do {
    received = ::read(fd, data, count);
    if (received >= 0) {
      return received;
    } else if (received == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::ReadEvent event(fd, bind(&Coroutine::run, &Coroutine::Running()));
      Yield();
    }
  } while (true);
}

ssize_t ReadFully(int fd, void* buf, size_t count) {
  uint8_t *data = static_cast<uint8_t*>(buf);
  size_t received = 0;
  ssize_t partial = 0;

  do {
    partial = Read(fd, data+received, count-received);
    if (partial > 0) {
      received += partial;
    } else if (partial == 0) {
      // No more data to read.
      return received;
    } else {
      // An error happened.
      return partial;
    }
  } while (received < count);

  return received;
}

ssize_t Write(int fd, const void *buf, size_t count) {
  ssize_t written = 0;

  do {
    written = ::write(fd, buf, count);
    if (written >= 0) {
      return written;
    } else if (written == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::WriteEvent event(fd, bind(&Coroutine::run, &Coroutine::Running()));
      Yield();
    }
  } while (true);
}

ssize_t WriteFully(int fd, const void* buf, size_t count) {
  const uint8_t *data = static_cast<const uint8_t*>(buf);
  size_t sent = 0;
  ssize_t partial = 0;

  do {
    partial = Write(fd, data+sent, count-sent);
    if (partial > 0) {
      sent += partial;
    } else if (partial == 0) {
      // No more data to read.
      return sent;
    } else {
      // An error happened.
      return partial;
    }
  } while (sent < count);

  return sent;
}

ssize_t Recv(int fd, void *buf, size_t len, int flags) {
  ssize_t added = 0;

  do {
    added = ::recv(fd, buf, len, flags);
    if (added >= 0) {
      return added;
    } else if (added == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::ReadEvent event(fd, bind(&Coroutine::run, &Coroutine::Running()));
      Yield();
    }
  } while (true);
}

ssize_t RecvFully(int fd, void *buf, size_t len, int flags) {
  uint8_t *data = static_cast<uint8_t*>(buf);
  size_t received = 0;
  ssize_t partial = 0;

  do {
    partial = Recv(fd, data+received, len-received, flags);
    if (partial > 0) {
      received += partial;
    } else if (partial == 0) {
      // No more data to read.
      return received;
    } else {
      // An error happened.
      return partial;
    }
  } while (received < len);

  return received;
}

ssize_t Send(int fd, const void* buf, size_t len, int flags) {
  ssize_t wrote = 0;

  do {
    wrote = ::send(fd, buf, len, flags);
    if (wrote >= 0) {
      return wrote;
    } else if (wrote == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::WriteEvent event(fd, bind(&Coroutine::run, &Coroutine::Running()));
      Yield();
    }
  } while (true);
}

ssize_t SendFully(int fd, const void* buf, size_t len, int flags) {
  const uint8_t *data = static_cast<const uint8_t*>(buf);
  size_t sent = 0;
  ssize_t partial = 0;

  do {
    partial = Send(fd, data+sent, len-sent, flags);
    if (partial > 0) {
      sent += partial;
    } else if (partial == 0) {
      // No more data to read.
      return sent;
    } else {
      // An error happened.
      return partial;
    }
  } while (sent < len);

  return sent;
}

int Socket(int domain, int type, int protocol) {
  int sock = ::socket(domain, type, protocol);
  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
  return sock;
}

int Connect(int fd, const struct sockaddr *addr, socklen_t addrlen) {
  return ::connect(fd, addr, addrlen);
}

int Bind(int fd, const struct sockaddr *addr, socklen_t addrlen) {
  return ::bind(fd, addr, addrlen);
}

int Listen(int fd, int backlog) {
  return ::listen(fd, backlog);
}

int Accept(int fd, struct sockaddr *addr, socklen_t *addrlen) {
  int sock;

  while (true) {
    sock = ::accept(fd, addr, addrlen);
    if (sock > 0) {
      return sock;
    } else if (sock == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
      return -1;
    } else {
      Event::ReadEvent event(fd, bind(&Coroutine::run, &Coroutine::Running()));
      Yield();
    }
  }
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

void CAIO_SetStackSize(size_t size) {
  return CAIO::SetStackSize(size);
}

ssize_t CAIO_Read(int fd, void* buf, size_t count) {
  return CAIO::Read(fd, buf, count);
}

ssize_t CAIO_ReadFully(int fd, void* buf, size_t count) {
  return CAIO::ReadFully(fd, buf, count);
}

ssize_t CAIO_Write(int fd, const void *buf, size_t count) {
  return CAIO::Write(fd, buf, count);
}

ssize_t CAIO_WriteFully(int fd, const void *buf, size_t count) {
  return CAIO::WriteFully(fd, buf, count);
}

ssize_t CAIO_Recv(int fd, void *buf, size_t len, int flags) {
  return CAIO::Recv(fd, buf, len, flags);
}

ssize_t CAIO_RecvFully(int fd, void *buf, size_t len, int flags) {
  return CAIO::RecvFully(fd, buf, len, flags);
}

ssize_t CAIO_Send(int fd, const void *buf, size_t len, int flags) {
  return CAIO::Send(fd, buf, len, flags);
}

ssize_t CAIO_SendFully(int fd, const void *buf, size_t len, int flags) {
  return CAIO::SendFully(fd, buf, len, flags);
}

int CAIO_Socket(int domain, int type, int protocol) {
  return CAIO::Socket(domain, type, protocol);
}

int CAIO_Connect(int fd, const struct sockaddr *addr, socklen_t addrlen) {
  return CAIO::Connect(fd, addr, addrlen);
}

int CAIO_Bind(int fd, const struct sockaddr *addr, socklen_t addrlen) {
  return CAIO::Bind(fd, addr, addrlen);
}

int CAIO_Listen(int fd, int backlog) {
  return CAIO::Listen(fd, backlog);
}

int CAIO_Accept(int fd, struct sockaddr *addr, socklen_t *addrlen) {
  return CAIO::Accept(fd, addr, addrlen);
}

int CAIO_Close(int fd) {
  return CAIO::Close(fd);
}
