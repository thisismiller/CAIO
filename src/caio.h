#ifndef _LIBCAIO_CAIO_HPP_
#define _LIBCAIO_CAIO_HPP_

#include <unistd.h>
#include <sys/socket.h>

#ifdef __cplusplus
#include <functional>

namespace CAIO {

void Spawn(std::function<void()> func);
void Yield();
void RunIOLoop();
void SetStackSize(size_t size);

ssize_t Read(int fd, void *buf, size_t count);
ssize_t ReadFully(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t WriteFully(int fd, const void *buf, size_t count);
ssize_t Recv(int fd, void *buf, size_t len, int flags);
ssize_t RecvFully(int fd, void *buf, size_t len, int flags);
ssize_t Send(int fd, const void *buf, size_t len, int flags);
ssize_t SendFully(int fd, const void *buf, size_t len, int flags);
int Socket(int domain, int type, int protocol);
int Connect(int fd, const struct sockaddr *addr, socklen_t addrlen);
int Bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *addr, socklen_t *addrlen);
int Close(int fd);

}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void CAIO_Spawn(void (*func)(void*), void* context);
void CAIO_Yield();
void CAIO_RunIOLoop();
void CAIO_SetStackSize(size_t size);

ssize_t CAIO_Read(int fd, void* buf, size_t count);
ssize_t CAIO_ReadFully(int fd, void* buf, size_t count);
ssize_t CAIO_Write(int fd, const void* buf, size_t count);
ssize_t CAIO_WriteFully(int fd, const void* buf, size_t count);
ssize_t CAIO_Recv(int fd, void *buf, size_t len, int flags);
ssize_t CAIO_RecvFully(int fd, void *buf, size_t len, int flags);
ssize_t CAIO_Send(int fd, const void *buf, size_t len, int flags);
ssize_t CAIO_SendFully(int fd, const void *buf, size_t len, int flags);
int CAIO_Socket(int domain, int type, int protocol);
int CAIO_Connect(int fd, const struct sockaddr *addr, socklen_t addrlen);
int CAIO_Bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
int CAIO_Listen(int fd, int backlog);
int CAIO_Accept(int fd, struct sockaddr *addr, socklen_t *addrlen);
int CAIO_Close(int fd);

#ifdef __cplusplus
}
#endif

#endif  /* _LIBCAIO_CAIO_HPP_ */
