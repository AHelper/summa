#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include "drv_summa.h"
#include "summa_child.h"

int running = 1;

static void catch_exit(int signo) {
  running = 0;
}

int main(int argc, char** argv) {
  char *dev_path = 0;
  char *resolution_code = 0;
  int daemonize = 0;
  int c;
  
  opterr = 0;
  
  while((c = getopt(argc, argv, "-dr:")) != -1) {
    switch(c) {
      case '\1':
        dev_path = optarg;
        break;
      case 'd':
        daemonize = 1;
        break;
      case 'r':
        resolution_code = optarg;
        break;
      case '?':
        if(optopt == 'r')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if(isprint(optopt))
          fprintf(stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character '\\x%X'.\n", optopt);
        return 1;
      default:
        return 1;
    }
  }
  summa_child_spawn("/var/run/drv_summa.socket");
  return 0;
  if(daemonize == 0) {
    if(dev_path == 0) {
      fprintf(stderr, "Device path required, exiting.\n");
      return 1;
    }
  
    signal(SIGINT, catch_exit);
    
    struct drv_summa_device dev;
    
    if(drv_summa_open(dev_path, &dev)) {
      fprintf(stderr, "drv_summa_open failed, %s\n", errstr);
      return 2;
    }
    
    if(resolution_code != 0) {
      const struct drv_summa_resolution* r = drv_summa_get_resolutions();
      
      while(r->code) {
        if(strcmp(r->code, resolution_code) == 0) {
          drv_summa_set_resolution(r, &dev);
          break;
        }
        r++;
      }
    }
    
    drv_summa_set_mouse_mode(0, &dev);
    
    while(running) {
      struct timeval tv;
      
      drv_summa_poll(&tv, &dev);
      
      select(0, 0, 0, 0, &tv);
    }
    
    printf("\n");
    drv_summa_close(&dev);
  } else {/*
    int sock = comm_summa_open("/var/run/drv_summa.socket");
    
    if(sock < 0) {
      fprintf(stderr, "Failed to open socket\n");
      return;
    }
    
    comm_summa_close(sock);*/
    summa_child_spawn("/var/run/drv_summa.socket");
  }
}