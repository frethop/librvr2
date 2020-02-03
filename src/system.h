#include "rvrio.h"
#include "rvrdefs.h"

#define GET_MAIN_APPLICATION_VERSION_COMMAND      0x00
#define GET_BOOTLOADER_VERSION_COMMAND            0x01
#define GET_BOARD_REVISION_COMMAND                0x03
#define GET_MAC_ADDRESS_COMMAND                   0x06
#define GET_STATS_ID_COMMAND                      0x13
#define GET_PROCESSOR_NAME_COMMAND                0x1F
#define GET_SKU_COMMAND                           0x38
#define GET_CORE_UP_TIME_IN_MILLISECONDS_COMMAND  0x39

u_int16_t * get_main_application_version();
u_int16_t * get_bootloader_version();
u_int8_t get_board_revision();
char * get_mac_address();
u_int16_t get_stats_id();
char * get_processor_name();
char * get_sku();
u_int64_t get_core_up_time_in_milliseconds();

