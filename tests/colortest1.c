//-------------------------------------------------------------------------------
//
//  Project One: use colors to direct driving.  
//
//  Drive forward until a color is detected, then change driving in the following
//  ways:
//     >>  

#include "drive.h"
#include "sensor.h"
#include "power.h"

#define TRUE 1

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define PURPLE 0x9933FF
#define YELLOW 0xFFFF00
#define WHITE  0xFFFFFF

void main(char **argv) {

    int color = -1;
    int old_color = -1;

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(NOLOGGING);

   wake();

   add_sensor(COLOR_DETECTION);
   configure_streaming_service();

   start_streaming_service(250);

   sleep(1);
   printf("R = %f, G = %f, B = %f\n", get_R(), get_G(), get_B());

   stop_streaming_service();
}

