#include "summa_child.h"
#include "drv_summa.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if 1
#define DEBUG
#endif

void summa_child_spawn(const char* path) {
  int pid = fork();
  
  if(pid != 0)
    return;
  
  // Child!
#ifdef DEBUG
  printf("Child spawned: %s\n", path);
#endif
  
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
  
  drv_summa_close(&dev);
}