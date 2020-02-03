#include <stdlib.h>
#include <stdio.h>
#include "rvrdefs.h"
#include "rvrio.h"

#define RAW_MOTOR_COMMAND                    0x01
#define RESET_YAW_COMMAND                    0x06
#define DRIVE_WITH_HEADING_COMMAND           0x07
#define ENABLE_MOTOR_STALL_NOTIFY_COMMAND    0x25
#define MOTOR_STALL_NOTIFY_COMMAND           0x26
#define ENABLE_MOTOR_FAULT_NOTIFY_COMMAND    0x27
#define MOTOR_FAULT_NOTIFY_COMMAND           0x28
#define GET_MOTOR_FAULT_STATE_COMMAND        0x29

#define RAW_MOTOR_MODE_OFF      0x00
#define RAW_MOTOR_MODE_FORWARD  0x01
#define RAW_MOTOR_MODE_REVERSE  0x02

#define LEFT_MOTOR  0x00
#define RIGHT_MOTOR 0x01

union drive_flags {
    u_int8_t allflags;
    struct flags {
        u_int8_t drive_reverse:1;
        u_int8_t boost:1;
        u_int8_t drive_fast:1;
        u_int8_t left_direction:1;
        u_int8_t right_direction:1;
        u_int8_t enable_drift:1;
        u_int8_t unused:2;
    } flag_bits;
};

#define DRIVE_FLAG_NO_FLAG         0x00
#define DRIVE_FLAG_DRIVE_REVERSE   0x01
#define DRIVE_FLAG_BOOST           0x02
#define DRIVE_FLAG_FAST_TURN       0x04
#define DRIVE_FLAG_LEFT_DIRECTION  0x08
#define DRIVE_FLAG_RIGHT_DIRECTION 0x10
#define DRIVE_FLAG_ENABLE_DRIFT    0x20

void drive_raw_motors(byte left_mode, byte left_speed, byte right_mode, byte right_speed);
void reset_yaw();
void drive_with_heading(byte speed, int heading, byte flags);

void enable_motor_stall_notify(int isEnabled);
u_int8_t * motor_stall_notify();
void enable_motor_fault_notify(int isEnabled);
int get_motor_fault_state();
void set_motor_stall_notify_data(u_int8_t *data);
u_int8_t get_motor_index();
int get_is_triggered();
void set_motor_fault_state(u_int8_t *data);