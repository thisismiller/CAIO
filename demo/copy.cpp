#include "aio.h"
#include <stdio.h>
#include <functional>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

void DoIO(int fd_in, int fd_out) {
  char buf[256];
  ssize_t read;

  do {
    read = AIO::Read(fd_in, buf, 256);
    if (read != -1) {
      AIO::Write(fd_out, buf, read);
    } else {
      printf("Error: %s\n", strerror(errno));
    }
  } while (read > 0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s [infile] [outfile]\n", argv[0]);
    return 1;
  }

  int in = open(argv[1], O_NONBLOCK|O_RDONLY);
  int out = open(argv[2], O_NONBLOCK|O_WRONLY|O_TRUNC|O_CREAT, 0751);

  AIO::Spawn(bind(DoIO, in, out));
  AIO::RunIOLoop();

  close(in);
  close(out);

  return 0;
}
