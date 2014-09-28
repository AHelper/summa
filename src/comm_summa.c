#include "comm_summa.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int comm_summa_open(const char* path)
{
  struct sockaddr_un local, remote;
  
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  
  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, path);
  unlink(local.sun_path);
  int len = strlen(path) + sizeof(local.sun_family);
  
  bind(sock, (struct sockaddr*)&local, len);
  
  listen(sock, 5);
  
  return sock;
}

void comm_summa_close(int fd)
{
  close(fd);
}

int comm_summa_accept(int fd)
{
  struct sockaddr_un remote;
  int t;
  
  return accept(fd, (struct sockaddr*)&remote, &t);
}
