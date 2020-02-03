#include "rvrio.h"
#include "power.h"
#include "system.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   //set_logging_level(VERBOSE);

   wake();

   u_int16_t * application_version = get_main_application_version();
   printf("*** Application version: %d.%d.%d\n", 
         application_version[0], application_version[1], application_version[2]);

   u_int16_t * bootloader = get_bootloader_version();
   printf("*** Bootloader version: %d.%d.%d\n", bootloader[0], bootloader[1], bootloader[2]);

   char * macaddress = get_mac_address();
   printf("*** MAC address is %s\n", macaddress);

   u_int8_t revision = get_board_revision();
   printf("*** Board revision is %d\n", revision);

   u_int16_t statsid = get_stats_id();
   printf("*** Stats ID is %d\n", statsid);

   char * procname = get_processor_name();
   printf("*** Processor name %s\n", procname);

   char * sku = get_sku();
   printf("*** SKU is %s\n", sku);

   long millis = get_core_up_time_in_milliseconds();
   float minutes = millis / 1000.0 / 60.0;
   printf("*** Uptime = %f minutes.\n", minutes);
}   
