#include "power.h"
#include "connection.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(VERBOSE);

   wake();

   char * btname = get_bluetooth_advertising_name();
   printf("BT name is %s'\n", btname);

   close_port();
}   
