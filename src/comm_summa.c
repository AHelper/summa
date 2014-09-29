#include "comm_summa.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

RET_CODE comm_summa_open(const char* path, int* sock)
{
  struct sockaddr_un local, remote;
  
  *sock = socket(AF_UNIX, SOCK_STREAM, 0);
  
  if(*sock == -1)
    RET(SOCKERR)
    
  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, path);
  
  if(unlink(local.sun_path) == -1)
    RET(PERM)
  
  int len = strlen(path) + sizeof(local.sun_family);
  
  if(bind(*sock, (struct sockaddr*)&local, len) == -1)
    RET(BIND)
  
  if(listen(*sock, 5) == -1)
    RET(LISTEN)
  
  RET(OK)
}

RET_CODE comm_summa_close(int fd)
{
  close(fd);
  
  RET(OK)
}

RET_CODE comm_summa_accept(int fd, int* client)
{
  struct sockaddr_un remote;
  int t;
  
  *client = accept(fd, (struct sockaddr*)&remote, &t);
  
  if(*client == -1) {
    if(errno == ECONNABORTED)
      RET(CLOSED)
    else
      RET(SOCKERR)
  } else {
    RET(OK)
  }
}

RET_CODE comm_summa_read_request(int fd, CommSummaRequest** request)
{
  uint8_t buf[1024];
  ssize_t len, total = 0;
  
  while(total != 1024 && (len = read(fd, buf + total, 1024 - total)) > 0) {
    total += len;
  }
  
  if(len == -1) {
    close(fd);
    RET(CLOSED)
  }
  
  *request = comm_summa_request__unpack(0, total, buf);
  
  if(*request == 0)
    RET(SOCKERR)
  else
    RET(OK)
}

RET_CODE comm_summa_send_query(int fd, const CommSummaQuery** query)
{

}

RET_CODE comm_summa_send_set(int fd, const CommSummaSet** set)
{

}
