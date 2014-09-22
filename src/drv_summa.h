#ifndef DRV_SUMMA_H
#define DRV_SUMMA_H

#include <time.h>
#include <stdint.h>
#include <linux/uinput.h>

#define RET_VAL_OK        0x00
#define RET_VAL_DEVERR    0x01
#define RET_VAL_IOERR     0x02
#define RET_VAL_UINPUTERR 0x03
#define RET_VAL_TTYERR    0x04
#define RET_VAL_TIMEOUT   0x05
#define RET_VAL_STATE     0x06

typedef uint16_t RET_CODE;

#define RET_STR_OK        ""
#define RET_STR_DEVERR    "Device failed to be accessed"
#define RET_STR_IOERR     "Device communication failure"
#define RET_STR_UINPUTERR "uinput failed to be accessed"
#define RET_STR_TTYERR    "TTY device failed to initialize"
#define RET_STR_TIMEOUT   "Device communication timeout"
#define RET_STR_STATE     "Invalid device driver state"

#define RET(x) {if(RET_VAL_OK != RET_VAL_##x) errstr = RET_STR_##x; return RET_VAL_##x;}

#define STATE_DISCONNECTED 0x00
#define STATE_CONNECTED    0x01
#define STATE_DIRTY        0x02

struct drv_summa_resolution {
  const char* code;
  uint16_t max_x;
  uint16_t max_y;
  const char* description;
};

struct drv_summa_prompt_data {
  unsigned phasing;
  unsigned proximity;
  unsigned id;
  unsigned sign_x;
  unsigned sign_y;
  unsigned buttons;
  signed x;
  signed y;
};

#define COMMAND_UNKNOWN ~0
#define COMMAND_TYPE     1
#define COMMAND_EVENTS   2
#define COMMAND_CALLBACK 3
  
struct drv_summa_command {
  int type;
  size_t size;
};

struct drv_summa_command_type {
  int type;
  size_t size;
  char* autotype;
};

struct drv_summa_command_events {
  int type;
  size_t size;
  struct input_event events[1];
};

struct drv_summa_command_callback {
  int type;
  size_t size;
  long data;
};

struct drv_summa_hotspot {
  int lower_x;
  int lower_y;
  int upper_x;
  int upper_y;
  struct drv_summa_command* commands;
};

struct drv_summa_viewport {
  struct {
    int lower_x;
    int lower_y;
    int upper_x;
    int upper_y;
  } device, screen;
};

struct drv_summa_device {
  char* dev_path; // Path to UART device
  int fd_dev;
  int fd_uinput_abs;
  int fd_uinput_rel;
  // TODO: State
  int state;
  int last_x, last_y;
  int cur_x,  cur_y;
  int last_buttons, cur_buttons;
  int last_proximity, cur_proximity;
  int use_relative;

  const struct drv_summa_resolution* current_res;
  struct uinput_user_dev uinput_dev;
  
  struct drv_summa_hotspot** hotspots;
  size_t num_hotspots;
  
  struct drv_summa_viewport** viewports;
  size_t num_viewports;
};

typedef void (*drv_summa_listener)(struct drv_summa_device* dev, struct drv_summa_command_callback* command);

extern const char* errstr;

const struct drv_summa_resolution* drv_summa_get_resolutions();
RET_CODE drv_summa_set_resolution(const struct drv_summa_resolution* res, struct drv_summa_device* dev);
RET_CODE drv_summa_close(struct drv_summa_device* dev);
RET_CODE drv_summa_open(char* dev_path, struct drv_summa_device* dev);
RET_CODE drv_summa_poll(struct timeval* wait_time, struct drv_summa_device* dev);
struct drv_summa_hotspot* drv_summa_get_hotspots(size_t* size, struct drv_summa_device* dev);
struct drv_summa_viewport* drv_summa_get_viewports(size_t* size, struct drv_summa_device* dev);
RET_CODE drv_summa_remove_hotspot(struct drv_summa_hotspot* hotspot, struct drv_summa_device* dev);
RET_CODE drv_summa_remove_viewport(struct drv_summa_viewport* viewport, struct drv_summa_device* dev);
RET_CODE drv_summa_add_hotspot(struct drv_summa_hotspot* hotspot, struct drv_summa_device* dev);
RET_CODE drv_summa_add_viewport(struct drv_summa_viewport* viewport, struct drv_summa_device* dev);
RET_CODE drv_summa_clear_hotspots(struct drv_summa_device* dev);
RET_CODE drv_summa_clear_viewports(struct drv_summa_device* dev);
RET_CODE drv_summa_set_mouse_mode(int enabled, struct drv_summa_device* dev);
// drv_summa_set_smart_button_detection -- Tracks buttons pressed to determine which exact buttons are pressed
RET_CODE drv_summa_map_button(int button, struct drv_summa_command* command, struct drv_summa_device* dev); // Pass 1 command
RET_CODE drv_summa_subscribe(drv_summa_listener listener, struct drv_summa_device* dev);
RET_CODE drv_summa_unsubscribe(drv_summa_listener listener, struct drv_summa_device* dev);

#endif // DRV_SUMMA_H
