#include "power.h"

void wake() {
    messageSend(WAKE_COMMAND, POWER_DEVICE_ID, NO_SOURCE, TARGET_ID_NORDIC, NULL, 0);
}

void fallasleep() {
    messageSend(SLEEP_COMMAND, POWER_DEVICE_ID, NO_SOURCE, TARGET_ID_NORDIC, NULL, 0);
}

u_int8_t get_battery_percentage() { 
    u_int8_t * percentage = messageSendAndRecv(GET_BATTERY_PERCENTAGE_COMMAND, POWER_DEVICE_ID, NO_SOURCE, TARGET_ID_NORDIC, NULL, 0, 1);
    return percentage[0];
} 

u_int8_t get_battery_voltage_state() {
    u_int8_t * state = messageSendAndRecv(GET_BATTERY_VOLTAGE_STATE_COMMAND, POWER_DEVICE_ID, NO_SOURCE, TARGET_ID_NORDIC, NULL, 0, 1);
    return *state;
}

float get_battery_voltage_in_volts(u_int8_t reading_type) {
    union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 
    u_int8_t * type_buffer = (u_int8_t *) malloc(sizeof(u_int8_t));   
    type_buffer[0] = reading_type;
    u_int8_t * voltage = 
        messageSendAndRecv(GET_BATTERY_VOLTAGE_IN_VOLTS_COMMAND, POWER_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                type_buffer, 1, 4);
    u_int32_t integer = voltage[0] << 24 | voltage[1] << 16 | voltage[2] << 8 | voltage[3];
    cvfloat.intvalue = integer;

    return cvfloat.floatvalue;
}

float *get_battery_voltage_state_thresholds() {
    union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 
    float * thresh_buffer = (float *) malloc(sizeof(float)*3);   
    u_int8_t * data = 
        messageSendAndRecv(GET_BATTERY_VOLTAGE_STATE_THRESHOLDS_COMMAND, POWER_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                NULL, 0, 12);

    u_int32_t integer = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    cvfloat.intvalue = integer;
    thresh_buffer[0] = cvfloat.floatvalue;
    integer = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];
    cvfloat.intvalue = integer;
    thresh_buffer[1] = cvfloat.floatvalue;
    integer = data[8] << 24 | data[9] << 16 | data[10] << 8 | data[11];
    cvfloat.intvalue = integer;
    thresh_buffer[3] = cvfloat.floatvalue;

    return thresh_buffer;
}

float get_current_sense_amplifier_current(u_int8_t amplifier_id) {
    union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 
    u_int8_t * amp_buffer = (u_int8_t *) malloc(sizeof(u_int8_t));   
    amp_buffer[0] = amplifier_id;
    u_int8_t * amps = 
        messageSendAndRecv(GET_CURRENT_SENSE_AMPLIFIER_CURRENT_COMMAND, POWER_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                amp_buffer, 1, 4);
    u_int32_t integer = amps[0] << 24 | amps[1] << 16 | amps[2] << 8 | amps[3];
    cvfloat.intvalue = integer;

    return cvfloat.floatvalue;
}

int will_sleep, did_sleep;

void set_will_sleep(u_int8_t *data) {
    will_sleep = data[0];
}

int get_will_sleep() { return will_sleep; }

void set_did_sleep(u_int8_t *data) {
    did_sleep = data[0];
}

int get_did_sleep() {return did_sleep; }

int battery_voltage_state_changed;

void set_battery_voltage_state_change(u_int8_t *data) {
    battery_voltage_state_changed = data[0];
}

int get_battery_voltage_state_changed() { return battery_voltage_state_changed; }
