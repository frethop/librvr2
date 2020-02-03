#include "rvrio.h"
#include "rvrdefs.h"

#define SET_ALL_LEDS_COMMAND                      0x1A
#define GET_ACTIVE_COLOR_PALETTE_COMMAND          0x44
#define SET_ACTIVE_COLOR_PALETTE_COMMAND          0x45
#define GET_COLOR_IDENTIFICATION_REPORT_COMMAND   0x46
#define LOAD_COLOR_PALETTE_COMMAND                0x47
#define SAVE_COLOR_PALETTE_COMMAND                0x48
#define RELEASE_LED_REQUESTS_COMMAND              0x4E

#define RIGHT_HEADLIGHT_RED_BITMASK            0x00000001
#define RIGHT_HEADLIGHT_GREEN_BITMASK          0x00000002
#define RIGHT_HEADLIGHT_BLUE_BITMASK           0x00000004
#define LEFT_HEADLIGHT_RED_BITMASK             0x00000008
#define LEFT_HEADLIGHT_GREEN_BITMASK           0x00000010
#define LEFT_HEADLIGHT_BLUE_BITMASK            0x00000020
#define LEFT_STATUS_INDICATION_RED_BITMASK     0x00000040
#define LEFT_STATUS_INDICATION_GREEN_BITMASK   0x00000080
#define LEFT_STATUS_INDICATION_BLUE_BITMASK    0x00000100
#define RIGHT_STATUS_INDICATION_RED_BITMASK    0x00000200
#define RIGHT_STATUS_INDICATION_GREEN_BITMASK  0x00000400
#define RIGHT_STATUS_INDICATION_BLUE_BITMASK   0x00000800
#define BATTERY_DOOR_REAR_RED_BITMASK          0x00001000
#define BATTERY_DOOR_REAR_GREEN_BITMASK        0x00002000
#define BATTERY_DOOR_REAR_BLUE_BITMASK         0x00004000
#define BATTERY_DOOR_FRONT_RED_BITMASK         0x00008000
#define BATTERY_DOOR_FRONT_GREEN_BITMASK       0x00010000
#define BATTERY_DOOR_FRONT_BLUE_BITMASK        0x00020000
#define POWER_BUTTON_FRONT_RED_BITMASK         0x00040000
#define POWER_BUTTON_FRONT_GREEN_BITMASK       0x00080000
#define POWER_BUTTON_FRONT_BLUE_BITMASK        0x00100000
#define POWER_BUTTON_REAR_RED_BITMASK          0x00200000
#define POWER_BUTTON_REAR_GREEN_BITMASK        0x00400000
#define POWER_BUTTON_REAR_BLUE_BITMASK         0x00800000
#define LEFT_BRAKELIGHT_RED_BITMASK            0x01000000
#define LEFT_BRAKELIGHT_GREEN_BITMASK          0x02000000
#define LEFT_BRAKELIGHT_BLUE_BITMASK           0x04000000
#define RIGHT_BRAKELIGHT_RED_BITMASK           0x08000000
#define RIGHT_BRAKELIGHT_GREEN_BITMASK         0x10000000
#define RIGHT_BRAKELIGHT_BLUE_BITMASK          0x20000000
#define UNDERCARRIAGE_WHITE_BITMASK            0x40000000

#define STATUS_INDICATOR_LEFT_GROUP   \
    LEFT_STATUS_INDICATION_RED_BITMASK |\
    LEFT_STATUS_INDICATION_GREEN_BITMASK |\
    LEFT_STATUS_INDICATION_BLUE_BITMASK
#define STATUS_INDICATOR_RIGHT_GROUP   \
    RIGHT_STATUS_INDICATION_RED_BITMASK |\
    RIGHT_STATUS_INDICATION_GREEN_BITMASK |\
    RIGHT_STATUS_INDICATION_BLUE_BITMASK
#define LEFT_HEADLIGHT_GROUP   \
    LEFT_HEADLIGHT_RED_BITMASK | LEFT_HEADLIGHT_GREEN_BITMASK | LEFT_HEADLIGHT_BLUE_BITMASK
#define RIGHT_HEADLIGHT_GROUP   \
    RIGHT_HEADLIGHT_RED_BITMASK | RIGHT_HEADLIGHT_GREEN_BITMASK | RIGHT_HEADLIGHT_BLUE_BITMASK
#define BATTERY_DOOR_FRONT   \
    BATTERY_DOOR_FRONT_RED_BITMASK | BATTERY_DOOR_FRONT_GREEN_BITMASK | BATTERY_DOOR_FRONT_BLUE_BITMASK
#define BATTERY_DOOR_REAR   \
    BATTERY_DOOR_REAR_RED_BITMASK | BATTERY_DOOR_REAR_GREEN_BITMASK | BATTERY_DOOR_REAR_BLUE_BITMASK
#define POWER_BUTTON_FRONT   \
    POWER_BUTTON_FRONT_RED_BITMASK | POWER_BUTTON_FRONT_GREEN_BITMASK | POWER_BUTTON_FRONT_BLUE_BITMASK
#define POWER_BUTTON_REAR   \
    POWER_BUTTON_REAR_RED_BITMASK | POWER_BUTTON_REAR_GREEN_BITMASK | POWER_BUTTON_REAR_BLUE_BITMASK
#define LEFT_BRAKELIGHT \
    LEFT_BRAKELIGHT_RED_BITMASK | LEFT_BRAKELIGHT_GREEN_BITMASK | LEFT_BRAKELIGHT_BLUE_BITMASK
#define RIGHT_BRAKELIGHT \
    RIGHT_BRAKELIGHT_RED_BITMASK | RIGHT_BRAKELIGHT_GREEN_BITMASK | RIGHT_BRAKELIGHT_BLUE_BITMASK
#define ALL_LIGHTS \
    STATUS_INDICATOR_LEFT_GROUP | STATUS_INDICATOR_RIGHT_GROUP |\
    LEFT_HEADLIGHT_GROUP | RIGHT_HEADLIGHT_GROUP |\
    BATTERY_DOOR_FRONT | BATTERY_DOOR_REAR |\
    POWER_BUTTON_FRONT | POWER_BUTTON_REAR |\
    LEFT_BRAKELIGHT | RIGHT_BRAKELIGHT
#define UNDERCARRIAGE_WHITE   UNDERCARRIAGE_WHITE_BITMASK

#define SPECDRUMS_COLOR_PALETTE_INDEX_DEFAULT   0x00
#define SPECDRUMS_COLOR_PALETTE_INDEX_MIDI      0x01

void set_all_leds(u_int32_t led_group, u_int8_t brightness[32]);
u_int8_t *get_active_color_palette();
void set_active_color_palette(u_int8_t *rgb_index_bytes);
u_int8_t *get_color_identification_report(u_int8_t red, u_int8_t green, u_int8_t blue, u_int8_t confidence_threshold);
void load_color_palette(u_int8_t palette_index);
void save_color_palette(u_int8_t palette_index);
void release_led_requests();