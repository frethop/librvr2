#include "sensor.h"
#include "power.h"

int main(char **argv) {
    float ambient_level;

    init_port();
    set_logging_level(VERBOSE);

    wake();
    sleep(2);

    ambient_level = get_ambient_light_sensor_value();
    printf("Ambient light level = %f\n", ambient_level);

}
