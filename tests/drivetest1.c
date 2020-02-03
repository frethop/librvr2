#include "rvrdefs.h"
#include "power.h"
#include "drive.h"

int main(char **argv) {
    init_port();
    set_logging_level(VERBOSE);

    wake();
    reset_yaw();

    drive_raw_motors(RAW_MOTOR_MODE_FORWARD, 128, RAW_MOTOR_MODE_FORWARD, 128);

    sleep(2);

    drive_raw_motors(RAW_MOTOR_MODE_FORWARD, 0, RAW_MOTOR_MODE_FORWARD, 0);
}
