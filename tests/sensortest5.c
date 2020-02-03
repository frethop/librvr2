#include "sensor.h"
#include "power.h"
#include "drive.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   //set_logging_level(VERBOSE);

   wake();
   sleep(2);

   printf("*** Sensing while driving ***\n");
 
   reset_yaw();
   reset_locator_x_and_y();
   add_sensor(LOCATOR);
   add_sensor(VELOCITY);
   add_sensor(SPEED);

   configure_streaming_service();
   start_streaming_service(250);
   sleep(1);
   printf("Locator = (%f, %f)\n", get_locator_X(), get_locator_Y());
   printf("Velocity = (%f, %f)\n", get_velocity_X(), get_velocity_Y());
   printf("Speed = %f\n", get_speed());
   printf("---------------------------------------\n");

   drive_with_heading(30, 0, 0);

   for (int i=0; i<3; i++) {
       sleep(1);
    	printf("Locator = (%f, %f)\n", get_locator_X(), get_locator_Y());
        printf("Velocity = (%f, %f)\n", get_velocity_X(), get_velocity_Y());
        printf("Speed = %f\n", get_speed());
        printf("---------------------------------------\n");
   } 

   drive_with_heading(30, 180, 0);
   for (int i=0; i<3; i++) {
       sleep(1);
    	printf("Locator = (%f, %f)\n", get_locator_X(), get_locator_Y());
        printf("Velocity = (%f, %f)\n", get_velocity_X(), get_velocity_Y());
        printf("Speed = %f\n", get_speed());
        printf("---------------------------------------\n");
   } 

   drive_with_heading(0,0,0);

   stop_streaming_service();
   clear_streaming_service(); 

}
