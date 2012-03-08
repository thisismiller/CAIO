#ifndef _LIBAIO_AIO_HPP_
#define _LIBAIO_AIO_HPP_

#include <unistd.h>

#ifdef __cplusplus
#include <functional>

namespace AIO {

void Spawn(std::function<void()> func);
void Yield();
void RunIOLoop();

ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);

}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void AIO_Spawn(void (*func)(void*), void* context);
void AIO_Yield();
void AIO_RunIOLoop();
ssize_t AIO_Read(int fd, void* buf, size_t count);
ssize_t AIO_Write(int fd, const void* buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif  /* _LIBAIO_AIO_HPP_ */
