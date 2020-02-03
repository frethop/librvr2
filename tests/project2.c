//-------------------------------------------------------------------------------
//
//  Project One: use colors to direct driving.  
//
//  Drive forward until a color is detected, then change driving in the following
//  ways:
//     >>  

#include <math.h>

#include "sensor.h"
#include "power.h"
#include "io.h"

#define TRUE 1

#define RED    187, 24, 0
#define GREEN  0, 91, 72
#define BLUE   0, 79, 154
#define PURPLE 0, 0, 28
#define YELLOW 251, 227, 5
#define WHITE  250, 245, 255

float color_distance(float Rx, float Gx, float Bx, float Ry, float Gy, float By) {
    float r = pow( Rx - Ry, 2);
    float g = pow( Gx - Gy, 2);
    float b = pow( Bx - By, 2);

    return sqrt(r + g + b);
}

void main(char **argv) {

    float r, g, b;
    float oldR = -1;
    float oldG = -1;
    float oldB = -1;
    float distance;

    byte colors[32];

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(NOLOGGING);

   wake();

   enable_color_detection(1);
   add_sensor(COLOR_DETECTION);
   configure_streaming_service();

   start_streaming_service(100);

   while (TRUE) {
        r = get_R();
        g = get_G();
        b = get_B();

        if (oldR != -1) {
           distance = color_distance(r, g, b, oldR, oldG, oldB);
           if (distance > 75) {
               printf("Got a new color!\n(%d, %d, %d) Distance = %f\n", (int)r, (int)g, (int)b, distance);

               for (int i=0; i<32; i++) colors[i] = 0x00;
               colors[0] = (int)r;
               colors[1] = (int)g;
               colors[2] = (int)b;
               colors[3] = (int)r;
               colors[4] = (int)g;
               colors[5] = (int)b;
               set_all_leds(LEFT_HEADLIGHT_GROUP | RIGHT_HEADLIGHT_GROUP, colors);               
           }
       }

       oldR = r;
       oldG = g;
       oldB = b;

   }

   stop_streaming_service();

}

