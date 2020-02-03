#include "drive.h"
#include "sensor.h"

void drive_turn_left(int degrees) {
    
}

void drive_turn_right(int degrees) {

}

void drive_turn_around() {

}

void drive_distance(float distance) {
    reset_locator_x_and_y();
    add_sensor(LOCATOR);
    configure_streaming_service();
    start_streaming_service(250);

    drive_with_heading(30,0,0);
    while (get_locator_X() < distance) ;
    drive_with_heading(0,0,0);

    stop_streaming_service();
    clear_streaming_service();
}
