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

    int speed = 60;
    float oldR = -1;
    float oldG = -1;
    float oldB = -1;
    float distance;

   /* Test for the first couple RVR commands */
   init_port();
   set_logging_level(NOLOGGING);

   wake();

   enable_color_detection(1);
   add_sensor(COLOR_DETECTION);
   configure_streaming_service();

   start_streaming_service(100);
   drive_with_heading(speed, 0, 0);

   while (TRUE) {
        if (oldR != -1) {
           distance = color_distance(get_R(), get_G(), get_B(), oldR, oldG, oldB);
           if (distance > 75) {
               printf("Got a new color!\n(%d, %d, %d) Distance = %f\n", (int)get_R(), (int)get_G(), (int)get_B(), distance);
               
               distance = color_distance(get_R(), get_G(), get_B(), RED);
               if (distance < 75) {
                   printf("...and it's RED!\n");
                   drive_with_heading(speed, 0, DRIVE_FLAG_RIGHT_DIRECTION);
               } else {
                   distance = color_distance(get_R(), get_G(), get_B(), GREEN);
                   if (distance < 75) {
                       printf("...and it's GREEN!\n"); 
                       drive_with_heading(speed, 0, DRIVE_FLAG_LEFT_DIRECTION);
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
                                   drive_with_heading(0, 0, 0);
                                   break;
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

