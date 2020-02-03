#include "api.h"
#include "power.h"

void main(char **argv) {

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(VERBOSE);

   wake();

   char * hello = echo("Hello World", strlen("Hello World"));
   printf("Sent 'Hello World' and got back '%s'\n", hello);

}   
