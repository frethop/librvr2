#include "sensor.h"
#include "power.h"
#include "drive.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   //set_logging_level(VERYVERBOSE);

   wake();

   reset_locator_x_and_y();
   //add_sensor(QUATERNION);
   //add_sensor(ACCELEROMETER);
   //add_sensor(IMU);
   //add_sensor(GYROSCOPE);
   //add_sensor(LOCATOR);
   //add_sensor(VELOCITY);
   //add_sensor(SPEED);
   //add_sensor(AMBIENT_LIGHT);
   add_sensor(COLOR_DETECTION);
   configure_streaming_service();

   //drive_with_heading(50, 0, 0);

   start_streaming_service(250);
   for (int i=0; i<5; i++) {
       sleep(1);
       //printf("Quaternion = (%f, %f, %f, %f)\n", get_quaternion_W(), get_quaternion_X(), get_quaternion_Y(), get_quaternion_Z());
       //printf("Accelerometer = (%f, %f, %f)\n", get_accelerometer_X(), get_accelerometer_Y(), get_accelerometer_Z());
       //printf("Roll: %f, Pitch: %f, Yaw: %f)\n", get_roll(), get_pitch(), get_yaw());
       //printf("Gyroscope = (%f, %f, %f)\n", get_gyroscope_X(), get_gyroscope_Y(), get_accelerometer_Z());
       //printf("Locator = (%f, %f)\n", get_locator_X(), get_locator_Y());
       //printf("Velocity = (%f, %f)\n", get_velocity_X(), get_velocity_Y());
       //printf("Speed = %f\n", get_speed());
       //printf("Light = %f\n", get_light());
       printf("Color = (%f, %f, %f)\n", get_R(), get_G(), get_B());
   }
   drive_with_heading(0,0,0);
   stop_streaming_service();

   clear_streaming_service(); 

}
