#include "io.h"
#include "power.h"

int main(char **argv) {
    byte off[32];
    byte red[32];
    byte green[32];

    init_port();
    set_logging_level(VERBOSE);

    wake();
    sleep(2);

    for (int i=0; i<32; i++) {
        off[i] = 0x00;
        red[i] = 0x00;
        green[i] = 0x00;
    }
    set_all_leds(ALL_LIGHTS, off);
    sleep(1);
    for (int i=0; i<32; i+=3) red[i] = 0xFF;
    for (int i=1; i<32; i+=3) green[i] = 0xFF;
    
    for (int i=0; i<10; i++) {
        set_all_leds(LEFT_BRAKELIGHT, red);
        sleep(1);
        set_all_leds(RIGHT_BRAKELIGHT, green);
        sleep(1);
        set_all_leds(LEFT_BRAKELIGHT, off);
        sleep(1);
        set_all_leds(RIGHT_BRAKELIGHT, off);
        sleep(1);
    }

}
