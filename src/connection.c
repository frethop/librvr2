#include "connection.h"

char * get_bluetooth_advertising_name() {
    char * btname = 
        messageSendAndRecv(GET_BLUETOOTH_ADVERT, CONNECTION_DEVICE_ID,
                           NO_SOURCE, TARGET_ID_NORDIC,
                           NULL, 0, 0);
    return btname;
}
