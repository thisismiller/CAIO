#include "caio.h"
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void client(int fd) {
  int read;
  char buf[256];

  do {
    read = CAIO::Recv(fd, buf, sizeof(buf), 0);
    if (read > 0)
      CAIO::SendFully(fd, buf, read, 0);
  } while (read > 0);

  CAIO::Close(fd);
}

void server() {
  int server_sock = CAIO::Socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in name;
  name.sin_family = AF_INET;
  name.sin_port = htons(31313);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  CAIO::Bind(server_sock, (const sockaddr*)&name, sizeof(name));
  CAIO::Listen(server_sock, 10);

  struct sockaddr addr;
  socklen_t addrlen;

  while(true) {
    int client_sock = CAIO::Accept(server_sock, &addr, &addrlen);
    CAIO::Spawn(std::bind(&client, client_sock));
  }
}

int main() {
  CAIO::Spawn(server);
  CAIO::RunIOLoop();
  return 0;
}
