#include "comm_summa.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdio.h>
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
    if(errno != ENOENT)
      RET(PERM)
  
  int len = strlen(path) + sizeof(local.sun_family);
  
  if(bind(*sock, (struct sockaddr*)&local, len) == -1)
    RET(BIND)
    
  chmod(path, 0777);
  
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
  *client = accept(fd, 0, 0);
  
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
  uint8_t buf[4];
  ssize_t len = 0, total = 0, pos = 0;
  
  while(total != 4)
    total += read(fd, buf + total, 4 - total);
  
  total = *(int*)buf;
  printf("%d\n", total);
  
  uint8_t buf2[total];
  
  while(pos != total && ((len = read(fd, buf2 + pos, total - pos)) >= 0)) {
    printf("read %d\n", pos);
    pos += len; // EBADF
  }
    printf("end %d\n", pos);
  
  if(len == -1) {
    close(fd);
    RET(CLOSED)
  }
  
  for(pos = 0; pos < total; pos++)
  {
    printf("%2X", buf2[pos]);
  }
  printf("\n");
  
  *request = comm_summa_request__unpack(0, total, buf2);
  
  if(*request == 0)
    RET(SOCKERR)
  else
    RET(OK)
}

RET_CODE comm_summa_send_response(int fd, const CommSummaResponse* response)
{
  size_t size;
  
  size = comm_summa_response__get_packed_size(response);
  
  uint8_t buf[size];
  
  comm_summa_response__pack(response, buf);
  printf("wrote %d\n", size);
  if(write(fd, (char*)&size, 4) == -1)
    RET(SOCKERR);
  if(write(fd, buf, size) == -1)
    RET(SOCKERR);
  fsync(fd);
  RET(OK);
}
