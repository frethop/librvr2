#include "sensor.h"
#include "power.h"
#include "drive.h"

int main(char **argv) {
    float temperatureL, temperatureR;

    init_port();
    //set_logging_level(VERBOSE);

    wake();
    sleep(2);

    get_motor_thermal_protection_status();
    temperatureL = get_left_motor_temperature();
    temperatureR = get_right_motor_temperature();
    printf("Left motor temp = %f\nRight motor temp = %f\n", 
        temperatureL, temperatureR);

    drive_raw_motors(RAW_MOTOR_MODE_FORWARD, 128, RAW_MOTOR_MODE_FORWARD, 128);
    sleep(5);
    drive_raw_motors(RAW_MOTOR_MODE_FORWARD, 0, RAW_MOTOR_MODE_FORWARD, 0);

    get_motor_thermal_protection_status();
    temperatureL = get_left_motor_temperature();
    temperatureR = get_right_motor_temperature();
    printf("Left motor temp = %f\nRight motor temp = %f\n", 
        temperatureL, temperatureR);

}
