#include "sensor.h"
#include "power.h"
#include "drive.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   //set_logging_level(VERBOSE);
   int flag = 0;

   wake();
   sleep(2);

   printf("*** One sensor at a time ***\n");
 
   for (int sensor=0; sensor < 8; sensor++) {
       flag = (flag == 0 ? DRIVE_FLAG_DRIVE_REVERSE : 0);
       drive_with_heading(30,0,flag);
       clear_streaming_service();
       sleep(1);
       reset_sensor_list();
       switch (sensor) {
          case 0:
    	      add_sensor(QUATERNION);
              break;
          case 1:
              add_sensor(ACCELEROMETER);
              break;
          case 2:
              add_sensor(IMU);
              break;
          case 3:
              add_sensor(GYROSCOPE);
              break;
          case 4:
              reset_locator_x_and_y();
              add_sensor(LOCATOR);
              break;
          case 5:
              add_sensor(VELOCITY);
              break;
          case 6:
              add_sensor(SPEED);
              break;
          case 7:
              add_sensor(AMBIENT_LIGHT);
              break;
       }
   
       configure_streaming_service();
       start_streaming_service(250);

       for (int i=0; i<3; i++) {
           sleep(1);
    	   switch (sensor) {
	       case 0:
               printf("Quaternion = (%f, %f, %f, %f)\n", get_quaternion_W(), get_quaternion_X(), get_quaternion_Y(), get_quaternion_Z());
	    	   break;
	       case 1:
               printf("Accelerometer = (%f, %f, %f)\n", get_accelerometer_X(), get_accelerometer_Y(), get_accelerometer_Z());
		       break;
	       case 2:
               printf("Roll: %f, Pitch: %f, Yaw: %f)\n", get_roll(), get_pitch(), get_yaw());
	           break;
		   case 3:
               printf("Gyroscope = (%f, %f, %f)\n", get_gyroscope_X(), get_gyroscope_Y(), get_accelerometer_Z());
		       break;
		   case 4:
               printf("Locator = (%f, %f)\n", get_locator_X(), get_locator_Y());
		       break;
		   case 5:
               printf("Velocity = (%f, %f)\n", get_velocity_X(), get_velocity_Y());
		       break;
		   case 6:
               printf("Speed = %f\n", get_speed());
		       break;
           case 7:
               printf("Light = %f\n", get_light());
               break;
           }
       }               
       stop_streaming_service();
       sleep(1);
   }
   drive_with_heading(0,0,0);
   clear_streaming_service(); 

}
