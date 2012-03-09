#ifndef _LIBCAIO_CAIO_HPP_
#define _LIBCAIO_CAIO_HPP_

#include <unistd.h>

#ifdef __cplusplus
#include <functional>

namespace CAIO {

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

void CAIO_Spawn(void (*func)(void*), void* context);
void CAIO_Yield();
void CAIO_RunIOLoop();

ssize_t CAIO_Read(int fd, void* buf, size_t count);
ssize_t CAIO_Write(int fd, const void* buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif  /* _LIBCAIO_CAIO_HPP_ */