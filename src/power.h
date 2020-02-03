#include "rvrio.h"
#include "rvrdefs.h"

#define SLEEP_COMMAND                        0x01 
#define WAKE_COMMAND                         0x0D
#define GET_BATTERY_PERCENTAGE_COMMAND       0x10
#define GET_BATTERY_VOLTAGE_STATE_COMMAND    0x17
#define WILL_SLEEP_NOTIFY_COMMAND            0x19
#define DID_SLEEP_NOTIFY_COMMAND             0x1A
#define ENABLE_BATTERY_VOLTAGE_STATE_CHANGE_NOTIFY_COMMAND 0x1B
#define BATTERY_VOLTAGE_STATE_CHANGE_NOTIFY_COMMAND        0x1C
#define GET_BATTERY_VOLTAGE_IN_VOLTS_COMMAND               0x25
#define GET_BATTERY_VOLTAGE_STATE_THRESHOLDS_COMMAND       0x26
#define GET_CURRENT_SENSE_AMPLIFIER_CURRENT_COMMAND        0x27

#define CALIBRATED_AND_FILTERED      0x0
#define CALIBRATED_AND_UNFILTERED    0x1
#define UNCALIBRATED_AND_UNFILTERED  0x2

#define BATTERY_VOLTAGE_STATE_UNKNOWN  0x0
#define BATTERY_VOLTAGE_STATE_OK       0x1
#define BATTERY_VOLTAGE_STATE_LOW      0x2
#define BATTERY_VOLTAGE_STATE_CRITICAL 0x3

#define LEFT_MOTOR_AMPLIFIER_ID  0x00
#define RIGHT_MOTOR_AMPLIFIER_ID 0x01

void wake();
void fallasleep();
u_int8_t get_battery_percentage();
u_int8_t get_battery_voltage_state();
float get_battery_voltage_in_volts(byte reading_type);
float *get_battery_voltage_state_thresholds();
float get_current_sense_amplifier_current(byte amplifier_id);
void set_will_sleep(u_int8_t *data);
int get_will_sleep();
void set_did_sleep(u_int8_t *data);
int get_did_sleep();
void set_battery_voltage_state_change(u_int8_t *data);
int get_battery_voltage_state_change();
