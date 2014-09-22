/*
 * User level SummaSketch II driver
 * 
 * GPLv2
 * 
 * Author: Collin Eggert <ahelper2@gmail.com>
 * 
 * Changelog:
 *      0.1     05/28/2014
 *              - initial version
 */
#include <termios.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include "drv_summa.h"

const char *errstr = RET_STR_OK;

#if 1
#define DEBUG
#else
#undef DEBUG
#endif

static void drv_summa_log(const char* str) {
  // TODO: syslog
#ifdef DEBUG
  printf("%s\n", str);
#endif
}

static RET_CODE drv_summa_init_tty(struct drv_summa_device *dev) {
  struct termios tty;
  
  // Clear
  memset(&tty, 0, sizeof(tty));
  
  if(tcgetattr(dev->fd_dev, &tty) != 0) {
    RET(TTYERR)
  }
  
  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);
  
  // Clear flags
  tty.c_lflag = 0;
  tty.c_oflag = 0;
  
  // Reset flags
  tty.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CRTSCTS);
  tty.c_iflag &= ~(IGNBRK | INLCR | ICRNL | IXOFF | IXON | IXANY);
  
  // Set flags
  tty.c_cflag |= CS8 | PARODD | CLOCAL | CREAD;
  
  // Set timing
  tty.c_cc[VMIN]  = 1;
  tty.c_cc[VTIME] = 10;
  
  if(tcsetattr(dev->fd_dev, TCSANOW, &tty) != 0) {
    RET(TTYERR)
  }
  
  RET(OK)
}

const struct drv_summa_resolution* drv_summa_get_resolutions() {
  const static struct drv_summa_resolution resolutions[]= {
    {"q", 12000, 12000, "1000 counts/in"}
  };
  
  return resolutions;
}

static RET_CODE drv_summa_write(const char* data, int len, struct drv_summa_device *dev) {
  ssize_t s;
  
  while((s = write(dev->fd_dev, data, len)) == -1 && errno == EINTR);
  
  if(s == -1) {
    RET(IOERR)
  } else {
    RET(OK)
  }
}

static RET_CODE drv_summa_read(char* data, int len, struct drv_summa_device *dev) {
  struct timeval timeout = {.tv_sec = 0, .tv_usec = 200000};
  fd_set set;
  int read_count = 0;
  int err;
  
  FD_ZERO(&set);
  FD_SET(dev->fd_dev, &set);
  
  while(read_count < len) {
    err = select(dev->fd_dev + 1, &set, 0, 0, &timeout);
    
    switch(err) {
      case -1: RET(IOERR)
      case  0: RET(TIMEOUT)
      default:
        err = read(dev->fd_dev, data + read_count++, 1);
        
        if(err == -1) {
          RET(IOERR)
        } else if(err == 0) {
          read_count--;
        }
    }
  }
  
  RET(OK)
}

RET_CODE drv_summa_set_resolution(const struct drv_summa_resolution *res, struct drv_summa_device *dev) {
  dev->current_res = res;
  
  if(dev->state == STATE_CONNECTED) {
    dev->state = STATE_DIRTY;
  }
}

RET_CODE drv_summa_close(struct drv_summa_device *dev) {
  drv_summa_log("Closing");
  
  if(dev->fd_dev >= 0) {
    drv_summa_write("\0", 1, dev);
    close(dev->fd_dev);
  }
  
  if(dev->fd_uinput_abs >= 0) {
    ioctl(dev->fd_uinput_abs, UI_DEV_DESTROY);
    
    close(dev->fd_uinput_abs);
  }
  
  if(dev->fd_uinput_rel >= 0) {
    ioctl(dev->fd_uinput_rel, UI_DEV_DESTROY);
    
    close(dev->fd_uinput_rel);
  }
  RET(OK)
}

RET_CODE drv_summa_open(char* dev_path, struct drv_summa_device *dev) {
  RET_CODE err;
  ssize_t s;
  
  memset(dev, 0, sizeof(struct drv_summa_device));
  dev->current_res = drv_summa_get_resolutions();
  
  // Access device
  dev->fd_dev = open(dev_path, O_RDWR | O_NOCTTY | O_SYNC);
  
  if(dev->fd_dev < 0) {
    RET(DEVERR)
  }
  
  // Access uinput
  dev->fd_uinput_rel = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  dev->fd_uinput_abs = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  
  if(dev->fd_uinput_rel < 0) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  if(dev->fd_uinput_abs < 0) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  // Set up device
  if(err = drv_summa_init_tty(dev)) {
    drv_summa_close(dev);
    return err;
  }
  
  // Set up uinput
  if(ioctl(dev->fd_uinput_rel, UI_SET_EVBIT, EV_REL) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_EVBIT, EV_KEY) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_KEYBIT, BTN_LEFT) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_KEYBIT, BTN_RIGHT) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_KEYBIT, BTN_MIDDLE) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_RELBIT, REL_WHEEL) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_RELBIT, REL_X) < 0 ||
    ioctl(dev->fd_uinput_rel ,UI_SET_RELBIT, REL_Y) < 0 ||
    ioctl(dev->fd_uinput_abs, UI_SET_EVBIT, EV_ABS) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_EVBIT, EV_KEY) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_KEYBIT, BTN_LEFT) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_KEYBIT, BTN_RIGHT) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_KEYBIT, BTN_MIDDLE) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_KEYBIT, BTN_WHEEL) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_KEYBIT, BTN_1) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_ABSBIT, ABS_X) < 0 ||
    ioctl(dev->fd_uinput_abs ,UI_SET_ABSBIT, ABS_Y) < 0) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  // Inject the device
  strncpy(dev->uinput_dev.name, "SummaSketch II", UINPUT_MAX_NAME_SIZE);
  dev->uinput_dev.id.bustype = BUS_RS232;
  dev->uinput_dev.id.vendor = 0;
  dev->uinput_dev.id.product = 1201;
  dev->uinput_dev.id.version = 2;
  dev->uinput_dev.absmin[ABS_X] = 0;
  dev->uinput_dev.absmax[ABS_X] = 12000; // FIXME: Need to translate
  dev->uinput_dev.absmin[ABS_Y] = 0;
  dev->uinput_dev.absmax[ABS_Y] = 12000; // FIXME: Again, translate
  
  s = write(dev->fd_uinput_rel, &dev->uinput_dev, sizeof(dev->uinput_dev));
  
  if(s == -1) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  s = write(dev->fd_uinput_abs, &dev->uinput_dev, sizeof(dev->uinput_dev));
  
  if(s == -1) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  if(ioctl(dev->fd_uinput_rel, UI_DEV_CREATE) < 0) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  if(ioctl(dev->fd_uinput_abs, UI_DEV_CREATE) < 0) {
    drv_summa_close(dev);
    RET(UINPUTERR)
  }
  
  drv_summa_log("Opened");
  RET(OK)
}

static RET_CODE drv_summa_uinput_emit(__u16 type, __u16 code, __s32 value, struct drv_summa_device *dev) {
  struct input_event ev;
  
  gettimeofday(&ev.time, 0);
  ev.type = type;
  ev.code = code;
  ev.value = value;
  
  if(write(dev->use_relative ? dev->fd_uinput_rel : dev->fd_uinput_abs, (char*)&ev, sizeof(ev)) == -1) {
    RET(UINPUTERR)
  } else {
    RET(OK)
  }
}

static RET_CODE drv_summa_uinput_syn(struct drv_summa_device *dev) {
  return drv_summa_uinput_emit(EV_SYN, SYN_REPORT, 0, dev);
}

static RET_CODE drv_summa_is_connected(struct drv_summa_device *dev) {
  RET_CODE err;
  char magic = (rand() % 127) + 1;
  char verify;
  
  if((err = drv_summa_write("\xEB", 1, dev)) != RET_VAL_OK)
    return err;
  
  usleep(200000);
  
  if((err = drv_summa_write(&magic, 1, dev)) != RET_VAL_OK)
    return err;
  
  if(drv_summa_read(&verify, 1, dev) == RET_VAL_OK && verify == magic) {
    RET(OK)
  } else {
    RET(IOERR)
  }
}

static RET_CODE drv_summa_dispatch(char *data, struct drv_summa_device *dev) {
  struct drv_summa_prompt_data prompt_data;
  int needs_syn = 0;
  
  // Parse the data
  prompt_data.phasing = (data[0] & 0b10000000) >> 7;
  prompt_data.proximity = (data[0] & 0b01000000) >> 6;
  prompt_data.id = (data[0] & 0b00100000) >> 5;
  prompt_data.sign_x = (data[0] & 0b00010000) >> 4;
  prompt_data.sign_y = (data[0] & 0b00001000) >> 3;
  prompt_data.buttons = data[0] & 0b00000111;
  prompt_data.x = (((data[2] & 0b01111111) << 7) | (data[1] & 0b01111111)) * (prompt_data.sign_x ? 1 : -1);
  prompt_data.y = (((data[4] & 0b01111111) << 7) | (data[3] & 0b01111111)) * (prompt_data.sign_y ? 1 : -1);
  
//   printf("%d%d%d%d%d%d %d,%d\n", prompt_data.phasing, prompt_data.proximity, prompt_data.id, prompt_data.sign_x, prompt_data.sign_y, prompt_data.buttons, prompt_data.x, prompt_data.y);
  
  if(prompt_data.phasing != 1) {
    RET(OK)
  } else {
    dev->last_proximity = dev->cur_proximity;
    dev->last_buttons = dev->cur_buttons;
    dev->last_x = dev->cur_x;
    dev->last_y = dev->cur_y;
    
    dev->cur_buttons = prompt_data.buttons;
    dev->cur_proximity = !prompt_data.proximity;
    dev->cur_x = prompt_data.x;
    dev->cur_y = dev->current_res->max_y - prompt_data.y;
    
    if(dev->cur_proximity != dev->last_proximity) {
      // TODO: Emit proximity change (IPC)
      if(dev->cur_proximity && dev->use_relative) {
        // Stop jumping
        dev->last_x = dev->cur_x;
        dev->last_y = dev->cur_y;
      }
    }
    
    if(dev->cur_proximity) {
      if(dev->cur_buttons != dev->last_buttons) {
        // TODO: Emit button change (IPC)
        // TODO: Check for hotspots
        // TODO: Check virtual viewports
        
        if((dev->cur_buttons | dev->last_buttons) == 0b001) {
          // TODO: Keymapping!!!
          drv_summa_uinput_emit(EV_KEY, BTN_LEFT, dev->cur_buttons == 0b001, dev);
          needs_syn = 1;
        }
        if((dev->cur_buttons | dev->last_buttons) == 0b010) {
          // TODO: Keymapping!!!
          drv_summa_uinput_emit(EV_KEY, BTN_RIGHT, dev->cur_buttons == 0b100, dev);
          needs_syn = 1;
        }
        if((dev->cur_buttons | dev->last_buttons) == 0b100) {
          // TODO: Keymapping!!!
          if(dev->cur_buttons == 0b010 && dev->last_buttons != 0b010)
            drv_summa_uinput_emit(EV_REL, REL_WHEEL, 1, dev);
          needs_syn = 1;
        }
        if((dev->cur_buttons | dev->last_buttons) == 0b011) {
          if(dev->cur_buttons == 0b011 && dev->last_buttons != 0b011)
            dev->use_relative = !dev->use_relative;
        }
      }
    
      if(dev->cur_x != dev->last_x) {
        if(dev->use_relative) {
          int diff = dev->cur_x - dev->last_x;
          
          if(diff >= 2 || diff <= -2)
            diff /= 2;
          drv_summa_uinput_emit(EV_REL, REL_X, diff, dev);
        } else {
          // TODO: Keymapping!!!
          drv_summa_uinput_emit(EV_ABS, ABS_X, dev->cur_x, dev);
        }
        needs_syn = 1;
      }
      if(dev->cur_y != dev->last_y) {
        if(dev->use_relative) {
          int diff = dev->cur_y - dev->last_y;
          
          if(diff >= 2 || diff <= -2)
            diff /= 2;
          
          drv_summa_uinput_emit(EV_REL, REL_Y, diff, dev);
        } else {
          // TODO: Keymapping!!!
          drv_summa_uinput_emit(EV_ABS, ABS_Y, dev->cur_y, dev);
        }
        needs_syn = 1;
      }
    }
    
    if(needs_syn) {
      drv_summa_uinput_syn(dev);
    }
  }
}

RET_CODE drv_summa_poll(struct timeval *wait_time, struct drv_summa_device *dev) {
  RET_CODE err;
  char prompt_buffer[5];
  
  wait_time->tv_sec = 0;
  wait_time->tv_usec = 0;
  
  switch(dev->state) {
    case STATE_DISCONNECTED:
      // Clear state
      drv_summa_write("\0", 1, dev);
      usleep(200000);
      
      // Flush input
      tcflush(dev->fd_dev, TCIFLUSH);
      
      // Is something there?
      if((err = drv_summa_is_connected(dev)) == RET_VAL_OK) {
        // Sets state to STATE_DIRTY
        dev->state = STATE_DIRTY;
      } else if(err != RET_VAL_IOERR) {
        wait_time->tv_sec = 1;
        return err;
      }
      break;
    case STATE_CONNECTED:
//       if((err = drv_summa_write("P", 1, dev)) != RET_VAL_OK) {
//         dev->state = STATE_DISCONNECTED;
//         drv_summa_log("SummaSketch disconnected, bad write");
//         return err;
//       }
      if((err = drv_summa_read(prompt_buffer, 5, dev)) != RET_VAL_OK) {
        drv_summa_write("P", 1, dev);
        if(drv_summa_read(prompt_buffer, 5, dev) != RET_VAL_OK) {
          dev->state = STATE_DISCONNECTED;
          drv_summa_log("SummaSketch disconnected, bad read length");
          return err;
        }
      }
      drv_summa_dispatch(prompt_buffer, dev);
      break;
    case STATE_DIRTY:
      // Clear state
      drv_summa_write("\0", 1, dev);
      usleep(200000);
      // Set resolution
      drv_summa_write(dev->current_res->code, strlen(dev->current_res->code), dev);
      usleep(200000);
      // Start auto-spam?
      if((err = drv_summa_write("\x32\x01", 2, dev)) != RET_VAL_OK) {
        dev->state = STATE_DISCONNECTED;
        drv_summa_log("SummaSketch disconnected, bad write");
        return err;
      }
      // Change state
      dev->state = STATE_CONNECTED;
      drv_summa_log("SummaSketch connected");
      break;
    default:
      RET(STATE)
      break;
  }
  
  RET(OK)
}

RET_CODE drv_summa_set_mouse_mode(int enabled, struct drv_summa_device* dev) {
  dev->use_relative = enabled;
  
  RET(OK)
}

RET_CODE drv_summa_clear_hotspots(struct drv_summa_device* dev) {
  free(dev->hotspots);
  dev->hotspots = 0;
  dev->num_hotspots = 0;
  
  RET(OK)
}

RET_CODE drv_summa_clear_viewports(struct drv_summa_device* dev) {
  free(dev->viewports);
  dev->viewports = 0;
  dev->num_viewports = 0;
  
  RET(OK)
}

RET_CODE drv_summa_add_hotspot(struct drv_summa_hotspot* hotspot, struct drv_summa_device* dev) {
  struct drv_summa_hotspot** copy = (struct drv_summa_hotspot**)malloc(sizeof(struct drv_summa_hotspot*) * ++dev->num_hotspots);
}
