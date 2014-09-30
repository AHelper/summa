#ifndef RET_CODE_H
#define RET_CODE_H

#define RET_VAL_OK        0x00
#define RET_VAL_DEVERR    0x01
#define RET_VAL_IOERR     0x02
#define RET_VAL_UINPUTERR 0x03
#define RET_VAL_TTYERR    0x04
#define RET_VAL_TIMEOUT   0x05
#define RET_VAL_STATE     0x06
#define RET_VAL_CLOSED    0x10
#define RET_VAL_SOCKERR   0x11
#define RET_VAL_PERM      0x12
#define RET_VAL_BIND      0x13
#define RET_VAL_LISTEN    0x14

typedef uint16_t RET_CODE;

#define RET_STR_OK        ""
#define RET_STR_DEVERR    "Device failed to be accessed"
#define RET_STR_IOERR     "Device communication failure"
#define RET_STR_UINPUTERR "uinput failed to be accessed"
#define RET_STR_TTYERR    "TTY device failed to initialize"
#define RET_STR_TIMEOUT   "Device communication timeout"
#define RET_STR_STATE     "Invalid device driver state"
#define RET_STR_CLOSED    "Network socket has been closed"
#define RET_STR_SOCKERR   "Unknown socket error"
#define RET_STR_PERM      "Insufficient privaleges"
#define RET_STR_BIND      "Unable to bind to socket"
#define RET_STR_LISTEN    "Unable to listen to socket"

#define RET(x) {if(RET_VAL_OK != RET_VAL_##x) errstr = RET_STR_##x; return RET_VAL_##x;}

extern const char* errstr;

#endif // RET_CODE_H