//-------------------------------------------------------------------------------
//
//  Project One: use colors to direct driving.  
//
//  Drive forward until a color is detected, then change driving in the following
//  ways:
//     >>  

#include <math.h>

#include "drive.h"
#include "sensor.h"
#include "power.h"

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

    float oldR = -1;
    float oldG = -1;
    float oldB = -1;
    float distance;

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(NOLOGGING);

   wake();

   add_sensor(COLOR_DETECTION);
   configure_streaming_service();

   start_streaming_service(250);

   while (1) {
       sleep(1);
       
       if (oldR != -1) {
           distance = color_distance(get_R(), get_G(), get_B(), oldR, oldG, oldB);
           if (distance > 75) {
               printf("Got a new color!\n");
               
               distance = color_distance(get_R(), get_G(), get_B(), RED);
               if (distance < 75) {
                   printf("...and it's RED!\n");
               } else {
                   distance = color_distance(get_R(), get_G(), get_B(), GREEN);
                   if (distance < 75) {
                       printf("...and it's GREEN!\n"); 
                   } else {
                       distance = color_distance(get_R(), get_G(), get_B(), BLUE);
                       if (distance < 75) {
                           printf("...and it's BLUE!\n"); 
                       } else {
                           distance = color_distance(get_R(), get_G(), get_B(), PURPLE);
                           if (distance < 75) {
                               printf("...and it's PURPLE!\n"); 
                           } else {
                               distance = color_distance(get_R(), get_G(), get_B(), YELLOW);
                               if (distance < 75) {
                                   printf("...and it's YELLOW!\n"); 
                               } else {
                                   distance = color_distance(get_R(), get_G(), get_B(), WHITE);
                                   if (distance < 75) {
                                       printf("...and it's WHITE!\n"); 
                                   }
                               }
                           }
                       }
                   }
               }
           }
       }

       oldR = get_R();
       oldG = get_G();
       oldB = get_B();

   }
   stop_streaming_service();

}

