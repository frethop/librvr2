#ifndef __RVRDEFS_H
#define __RVRDEFS_H 1

#include <sys/types.h>

union flag {
    u_int8_t allbits;
    struct bits {
        unsigned int is_response:1;
        unsigned int response_requested:1;
        unsigned int response_requested_if_error:1;
        unsigned int is_activity:1;
        unsigned int has_target:1;
        unsigned int has_source:1;
        unsigned int UNUSED:1;
        unsigned int has_more_flags:1;
    } flag_bits;
};

union locationID {
    u_int8_t wholeID;
    struct {
        unsigned int port:4;
        unsigned int node:4;
    } parts;
};

struct header {
    union flag flags;
    union locationID targetID;     /* optional, depends on flags */
    union locationID sourceID;     /* optional, depends on flags */
    u_int8_t deviceID;     /* command group ... see defines below */
    u_int8_t commandID;
    u_int8_t sequence_num;
    u_int8_t error_code;
};

#define MESSAGE_START 0x8D
#define MESSAGE_END   0xD8

struct message {
    struct header * msghdr;
    u_int8_t * payload;
    int loadlength;
};


#define NO_SOURCE       0x00
#define NO_DESTINATION  0x00

#define API_AND_SHELL_DEVICE_ID   0x10
#define SYSTEM_INFO_DEVICE_ID     0x11
#define POWER_DEVICE_ID           0x13
#define DRIVE_DEVICE_ID           0x16
#define SENSOR_DEVICE_ID          0x18
#define CONNECTION_DEVICE_ID      0x19
#define IO_DEVICE_ID              0x1A

#define TARGET_ID_NORDIC          0x01
#define TARGET_ID_ST              0x02

#define ERROR_SUCCESS             0x00
#define ERROR_BAD_DID             0x01
#define ERROR_BAD_CID             0x02
#define ERROR_NOT_YES_IMPLEMENTED 0x03
#define ERROR_RESTRICTED          0x04
#define ERROR_BAD_DATA_LENGTH     0x05
#define ERROR_FAILED              0x06
#define ERROR_BAD_DATA_VALUE      0x07
#define ERROR_BUSY                0x08
#define ERROR_BAD_TID             0x09
#define ERROR_TARGET_UNAVAILABLE  0x0A

#define NOLOGGING    0
#define BYTESINFO    1
#define VERBOSE      2
#define VERYVERBOSE  3

#define byte u_int8_t

#endif
