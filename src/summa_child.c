#include "summa_child.h"
#include "drv_summa.h"

#include <stdio.h>
#include <unistd.h>

void summa_child_spawn(const char* path) {
  int pid = fork();
  
  if(
}