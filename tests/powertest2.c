#include "power.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(BYTESINFO);

   wake();

   float volts = get_battery_voltage_in_volts(0);
   printf("Voltage reading -- calibrated and filtered -- is %f\n", volts);
   volts = get_battery_voltage_in_volts(1);
   printf("Voltage reading -- calibrated and unfiltered -- is %f\n", volts);
   volts = get_battery_voltage_in_volts(2);
   printf("Voltage reading -- uncalibrated and unfiltered -- is %f\n", volts);

}   
