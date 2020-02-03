#include "io.h"

void set_all_leds(u_int32_t led_group, u_int8_t led_brightness_values[32]) {
    int ones = 0;
    for (int i=0; i<32; i++) ones += (led_group >> i & 1);

    u_int8_t * buffer = (u_int8_t *) malloc(sizeof(u_int8_t)*(4+ones));

    buffer[0] = led_group >> 24 & 0xFF;
    buffer[1] = led_group >> 16 & 0xFF;
    buffer[2] = led_group >> 8 & 0xFF;
    buffer[3] = led_group & 0xFF;
    for (int i=4; i<4+ones; i++) buffer[i] = led_brightness_values[i-4];
    messageSend(SET_ALL_LEDS_COMMAND, IO_DEVICE_ID, 
                NO_SOURCE, TARGET_ID_NORDIC,
                buffer, 4+ones);
}

u_int8_t *get_active_color_palette() {
    u_int8_t *rgb_index_bytes = 
        messageSendAndRecv(GET_ACTIVE_COLOR_PALETTE_COMMAND, IO_DEVICE_ID,
                           NO_SOURCE, TARGET_ID_NORDIC, 
                           NULL, 0, 48);
    return rgb_index_bytes;

}

void set_active_color_palette(u_int8_t *rgb_index_bytes) {
    messageSend(SET_ACTIVE_COLOR_PALETTE_COMMAND, IO_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC, 
                rgb_index_bytes, 48);
}

u_int8_t *get_color_identification_report(u_int8_t red, u_int8_t green, u_int8_t blue, u_int8_t confidence_threshold) {
    u_int8_t *data = (u_int8_t *) malloc(sizeof(u_int8_t)*4);
    data[0] = red;
    data[1] = green;
    data[2] = blue;
    data[3] = confidence_threshold;

    u_int8_t *index_confidence_byte = 
        messageSendAndRecv(GET_COLOR_IDENTIFICATION_REPORT_COMMAND, IO_DEVICE_ID,
                           NO_SOURCE, TARGET_ID_NORDIC,
                           data, 4, 24);

    return index_confidence_byte;
}

void load_color_palette(u_int8_t palette_index) {
    u_int8_t *data = (u_int8_t *) malloc(sizeof(u_int8_t));
    data[0] = palette_index;
    messageSend(LOAD_COLOR_PALETTE_COMMAND, IO_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                data, 1);
}

void save_color_palette(u_int8_t palette_index) {
    u_int8_t *data = (u_int8_t *) malloc(sizeof(u_int8_t));
    data[0] = palette_index;
    messageSend(LOAD_COLOR_PALETTE_COMMAND, IO_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                data, 1);
}

void release_led_requests() {
    messageSend(RELEASE_LED_REQUESTS_COMMAND, IO_DEVICE_ID, 
                NO_SOURCE, TARGET_ID_NORDIC,
                NULL, 0);
}



