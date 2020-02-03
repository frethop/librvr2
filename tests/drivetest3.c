#include "rvrdefs.h"
#include "power.h"
#include "drive.h"
#include "sensor.h"

int main(char **argv) {
    char c;

    init_port();
    //set_logging_level(VERBOSE);

    printf("*** Test driving with heading ***\n");

    wake();
    reset_yaw();
    
    printf("*** Forward for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, 0);
    sleep(2);
    drive_with_heading(0, 0, 0);

    printf("*** Reverse for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, DRIVE_FLAG_DRIVE_REVERSE);
    sleep(2);
    drive_with_heading(0, 0, 0);

    printf("*** Heading in left direction for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, DRIVE_FLAG_LEFT_DIRECTION);
    sleep(2);
    drive_with_heading(0, 0, 0);

    printf("*** Forward for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, 0);
    sleep(2);
    drive_with_heading(0, 0, 0);

    printf("*** Heading in right direction for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, DRIVE_FLAG_RIGHT_DIRECTION);
    sleep(2);
    drive_with_heading(0, 0, 0);

    printf("*** Forward for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, 0);
    sleep(2);
    drive_with_heading(0, 0, 0);

    printf("*** Driving backwards for 2 seconds ***\n");
    printf("*** Press enter to start");
    scanf("%c", &c);
    drive_with_heading(50, 0, DRIVE_FLAG_DRIVE_REVERSE);
    sleep(2);
    drive_with_heading(0, 0, 0);

    
}
