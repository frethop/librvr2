#include "rvrdefs.h"
#include "power.h"
#include "drive.h"
#include "sensor.h"

int main(char **argv) {
    init_port();
    //set_logging_level(VERBOSE);

    printf("*** Test driving with heading ***\n");

    wake();
    reset_yaw();
    reset_locator_x_and_y();

    add_sensor(LOCATOR);
    configure_streaming_service();
    start_streaming_service(250);
    
    drive_with_heading(50, 0, 0);
    sleep(1);
    printf("(%f, %f)\n", get_locator_X(), get_locator_Y());

    drive_with_heading(50, 0, DRIVE_FLAG_DRIVE_REVERSE);
    sleep(1);
    printf("(%f, %f)\n", get_locator_X(), get_locator_Y());

    drive_with_heading(50, 90, 0);
    sleep(1);
    printf("(%f, %f)\n", get_locator_X(), get_locator_Y());

    drive_with_heading(50, 270, 0);
    sleep(1);
    printf("(%f, %f)\n", get_locator_X(), get_locator_Y());

    drive_with_heading(0, 0, 0);
    printf("(%f, %f)\n", get_locator_X(), get_locator_Y());


    stop_streaming_service();
    clear_streaming_service();

}
