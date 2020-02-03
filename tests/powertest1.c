#include "power.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   //set_logging_level(VERBOSE);

   wake();

   int batts = get_battery_percentage();
   printf("Battery %%age reported as %d%%\n", batts);

   int state = get_battery_voltage_state();
   printf("Battery state is %d\n", state);
}   
