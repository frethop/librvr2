#include "rvrio.h"
#include "sensor.h"
#include "drive.h"
#include "power.h"
#include "io.h"

int zeemote_fd;
int data_pipe[2];
int sequence_number;
int reading;

byte zcode, zcmd, button;
byte zm, zx, zy;

pthread_t input_thread;

int infrared_code;

#define TRUE 1
#define FALSE 0
#define READ  0
#define WRITE 1

#define JOYSTICK 0x05
#define BUTTON  0x08

#define BUTTON_A  0x00
#define BUTTON_B  0x01
#define BUTTON_C  0x02
#define BUTTON_D  0x03
#define BUTTON_POWER BUTTON_D
#define BUTTON_RELEASE   0xfe

#define NOTHING 0xfe

/*
 * Used stackoverflow for this, because it was easy:
 * https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
 */

int
set_interface_attribs1 (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf (stderr, "error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                fprintf (stderr, "error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking1 (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf (stderr, "error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                fprintf (stderr, "error %d setting term attributes", errno);
}

#define FORWARD 0
#define REVERSE 1

void main(char **argv) {

    byte colors[32];
    int direction, heading;
    int speed;
    int prevX, prevY, diff;

    char *portname = "/dev/rfcomm0";
    zeemote_fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (zeemote_fd < 0) {
        fprintf (stderr, "error %d opening %s: %s", errno, portname, strerror (errno));
        return;
    }

    set_interface_attribs1 (zeemote_fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking1 (zeemote_fd, 1); 

    init_port();
    wake();
    reset_yaw();
    set_logging_level(NOLOGGING);

    for (int i=0; i<32; i++) colors[i] = 0x00;  
    set_all_leds(ALL_LIGHTS, colors); 

    direction = DRIVE_FLAG_NO_FLAG;              
    speed = 50;
    heading = 0;
    prevX = prevY = 0;

    while (1) {
        read(zeemote_fd, &zcode, 1);
        zcmd = zcode;
        printf("COMMAND: %2x\n", zcmd);
        read(zeemote_fd, &zcode, 1);  // 0xa1
        printf("...filler: %2x\n", zcode);
        read(zeemote_fd, &zcode, 1);  // 0x07
        printf("...filler: %2x\n", zcode);
        if (zcmd == JOYSTICK) {
            read(zeemote_fd, &zm, 1); 
            read(zeemote_fd, &zx, 1); 
            read(zeemote_fd, &zy, 1); 
            printf("JOYSTICK: (%2x, %2x)\n", zx, zy);

            diff = zx - prevX;
            if (zx < 0x80) {
                if (diff > 0) heading += diff/2;
            } else {
                if (diff < 0) heading += diff/2;
            }
            if (diff > 360) heading -= 360;
            if (diff < 0) heading += 360; 

            diff = zy - prevY;
            if (zy > 0x7F) {
                if (diff < 0) speed += (-diff/2);
            } else {
                if (diff > 0) speed += (-diff/2);
            }
            if (speed > 255) speed = 255;
            prevY = zy;

            drive_with_heading(speed, heading, direction);

        } else if (zcmd == BUTTON) {
            read(zeemote_fd, &zcode, 1); 
            button = zcode;
            printf("BUTTON: %2x\n", button);
            if (button == BUTTON_RELEASE) {
                printf("BUTTON RELEASE\n");
            } else if (button == BUTTON_A) {
                printf("BUTTON A\n");
                drive_with_heading(0,0,0);
                direction = DRIVE_FLAG_NO_FLAG;
                drive_with_heading(speed, heading, direction);
            } else if (button == BUTTON_B) {
                printf("BUTTON B\n");
                drive_with_heading(0,0,0);
                direction = DRIVE_FLAG_DRIVE_REVERSE;
                drive_with_heading(speed, heading, direction);
            } else if (button == BUTTON_C) {
                printf("BUTTON C\n");
                colors[0] = 0xFF;
                colors[4] = 0xFF;
                set_all_leds(ALL_LIGHTS, colors); 

                drive_with_heading(0,0,0);
                if (speed == 0) speed = 50;
                drive_with_heading(speed, heading, direction);

            } else if (button == BUTTON_D) {
                printf("BUTTON D\n");
                for (int i=0; i<32; i++) colors[i] = 0x00;  
                set_all_leds(ALL_LIGHTS, colors);  
                drive_with_heading(0,0,0);
                speed = 0; 
                heading = 0;
                reset_yaw();            
            } else {
                printf("*** ERROR: UNKNOWN BUTTON\n");
            }
            // filler
            for (int i=0; i<5; i++) read(zeemote_fd, &zcode, 1);

        }
        
    }
}