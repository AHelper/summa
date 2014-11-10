#include "summa_child.h"
#include "drv_summa.h"
#include "comm_summa.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#if 1
#define DEBUG
#endif

void summa_child_spawn(const char* path) {
  int opened = 0;
//   int pid = fork();
  int client_socks[5];
  RET_CODE ret;
  int sock;
  int i;
  struct drv_summa_device dev;

//   if(pid != 0)
//     return;

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
  
  for(i = 0; i < 5; client_socks[i++] = 0);

  while(1) {
    select(maxset + 1, &set, 0, 0, 0);
printf("Event\n");
    if(FD_ISSET(sock, &set)) {
      int new_client;
      ret = comm_summa_accept(sock, &new_client);

      if(ret) {
        fprintf(stderr, "Error accepting client, %s\n", errstr);
      } else {
        printf("New client\n");

        for(i = 0; i < 5; i++) {
          if(client_socks[i] == 0) {
            client_socks[i] = new_client;
            maxset = new_client > maxset ? new_client : maxset;
            FD_SET(new_client, &set);
            break;
          }
        }

        if(i == 5) {
          close(new_client); // Rejected, too many connections
          printf("Rejected\n");
        }
      }
    } else {
      printf("Client event\n");
      int i;
      
      for(i = 0; i < 5; i++) {
        if(FD_ISSET(client_socks[i], &set)) {
          CommSummaRequest* r;
          RET_CODE ret = comm_summa_read_request(client_socks[i], &r);

          if(ret)
            fprintf(stderr, "Recv error, %s\n", errstr);
          else {
            CommSummaResponse res;
            
            comm_summa_response__init(&res);
            printf("Got a request!\n");

            if(r->device) {
              if(strlen(r->device) == 0) {
                if(opened) {
                  drv_summa_close(&dev);
                  opened = 0;
                  res.device_ok = 0;
                }
              } else {
                if(opened) {
                  drv_summa_close(&dev);
                }
                if(drv_summa_open(r->device, &dev)) {
                  opened = res.device_ok = 0;
                } else {
                  opened = res.device_ok = 1;
                }
              }
            } else {
              res.device_ok = opened;
            }
            res.has_device_ok = 1;
            
            if(r->has_query) {
              if(r->query) {
                res.query = malloc(sizeof(CommSummaQuery));
                comm_summa_query__init(res.query);
                
                res.query->set = malloc(sizeof(CommSummaSet));
                comm_summa_set__init(res.query->set);
                
                res.query->set->dpi = 123;
                res.query->set->has_dpi = 1;
                
                res.query->n_dpi = 0;
                res.query->has_absolute = 0;
                res.query->has_anti_jitter = 0;
                res.query->has_relative = 0;
                res.query->has_smart_buttons = 0;
                ret = comm_summa_send_response(client_socks[i], &res);
                if(ret)
                  fprintf(stderr, "Send error, %s\n", errstr);
                else
                  printf("Sent!\n");
              }
            }
            printf("Done\n");
          }
        }
      }
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
