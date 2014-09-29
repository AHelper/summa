#include "summa_child.h"
#include "drv_summa.h"
#include "comm_summa.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if 1
#define DEBUG
#endif

void summa_child_spawn(const char* path) {
  int opened = 0;
  int pid = fork();
  int client_socks[5];
  RET_CODE ret;
  int sock;

  if(pid != 0)
    return;

  fd_set set;
  int maxset = 0;

  FD_ZERO(&set);

  // Child!
#ifdef DEBUG
  printf("Child spawned: %s\n", path);
#endif

  ret = comm_summa_open(path, &sock);


  if(ret) {
    fprintf(stderr, "comm_summa_open failed, %s\n", errstr);
    return;
  }

  FD_SET(sock, &set);
  maxset = sock > maxset ? sock : maxset;

  while(1) {
    if(!opened) {
      select(maxset + 1, &set, 0, 0, 0);

      if(FD_ISSET(sock, &set)) {
        int new_client;
        ret = comm_summa_accept(sock, &new_client);

        if(ret) {
          fprintf(stderr, "Error accepting client, %s\n", errstr);
        } else {
          int i;

          for(i = 0; i < 5; i++) {
            if(client_socks[i] == 0) {
              client_socks[i] = new_client;
              FD_SET(new_client, &set);
            }
          }

          if(i == 5) {
            close(new_client); // Rejected, too many connections
          }
        }
      } else {
        int i;
        
        for(i = 0; i < 5; i++) {
          if(FD_ISSET(client_socks[i], &set)) {
            CommSummaRequest* r;
            RET_CODE ret = comm_summa_read_request(client_socks[i], &r);

            if(ret)
              fprintf(stderr, "Recv error, %s\n", errstr);
            else {
              printf("Got a request!\n");

              if(r->query) {
                printf("Client queried me!\n");
              }
            }
          }
        }
      }
    } else {

    }
  }

  /*
  struct drv_summa_device dev;

  if(drv_summa_open(path, &dev)) {
    fprintf(stderr, "drv_summa_open failed, %s\n", errstr);
    return;
  }

  drv_summa_set_mouse_mode(0, &dev);

  while(1) {
    struct timeval tv;

    drv_summa_poll(&tv, &dev);

    select(0, 0, 0, 0, &tv);
  }

  drv_summa_close(&dev);*/


}
