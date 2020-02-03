#include "sensor.h"
#include "power.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(BYTESINFO);

   wake();

   clear_streaming_service();

   configure_streaming_service();  // nothing added

   add_sensor(ACCELEROMETER);
   configure_streaming_service();

   start_streaming_service(250);
   for (int i=0; i<5; i++) {
       sleep(1);
       printf("Accelerometer = (%f, %f, %f)\n", get_accelerometer_X(), get_accelerometer_Y(), get_accelerometer_Z());
   }
   stop_streaming_service();

   clear_streaming_service();

}