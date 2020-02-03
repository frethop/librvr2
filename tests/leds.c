#include "io.h"
#include "power.h"

int main(char **argv) {
    byte colors[32];

    init_port();
    set_logging_level(VERBOSE);

    wake();
    sleep(2);

    for (int i=0; i<32; i++) colors[i] = 0x00;
    set_all_leds(ALL_LIGHTS, colors);
    sleep(1);
    for (int i=1; i<32; i+=3) colors[i] = 0xFF;
    set_all_leds(ALL_LIGHTS, colors);

    sleep(1);
    for (int i=0; i<32; i++) colors[i] = 0x00;
    set_all_leds(ALL_LIGHTS, colors);
    colors[0] = 0xFF;
    set_all_leds(LEFT_HEADLIGHT_GROUP, colors);
}
