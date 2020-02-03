#include "rvrio.h"
#include "sensor.h"
#include "drive.h"
#include "power.h"

int serial_port_fd;
int data_pipe[2];
int sequence_number;
int reading;

int port_configured = 0;

pthread_t input_thread;

int infrared_code;

#define TRUE 1
#define FALSE 0
#define READ  0
#define WRITE 1

void write_message(int fd, struct message * data);

/*
 * Used stackoverflow for this, because it was easy:
 * https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
 */

int
set_interface_attribs (int fd, int speed, int parity)
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
set_blocking (int fd, int should_block)
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

void *read_thread() {

    struct message *msg;
    u_int8_t *sensor_data, *temperature_data;

    reading = 1;
    while (TRUE) { 
        msg = read_message(serial_port_fd);

        if (msg->msghdr->deviceID == SENSOR_DEVICE_ID) {
            if (msg->msghdr->commandID == GYRO_MAX_NOTIFY) {
                set_gyro_flags(*(msg->payload));
            } else if (msg->msghdr->commandID == ROBOT_TO_ROBOT_INFRARED_MESSAGE_RECEIVED_NOTIFY) {
                infrared_code = *(msg->payload);
            } else if (msg->msghdr->commandID == COLOR_DETECTION_NOTIFY) {
                set_color_data(msg->payload);
            } else if (msg->msghdr->commandID == STREAMING_SERVICE_DATA_NOTIFY) {
                sensor_data = msg->payload;
                set_sensor_data(sensor_data);
            } else if (msg->msghdr->commandID == MOTOR_THERMAL_PROTECTION_STATUS_NOTIFY) {
                temperature_data = msg->payload;
            } else {
                write_message(data_pipe[WRITE], msg);
            }
        } else if (msg->msghdr->commandID == MOTOR_STALL_NOTIFY_COMMAND) {
            set_motor_stall_notify_data(msg->payload);
        } else if (msg->msghdr->commandID == MOTOR_FAULT_NOTIFY_COMMAND) {
            set_motor_fault_state(msg->payload);
        } else if (msg->msghdr->commandID == WILL_SLEEP_NOTIFY_COMMAND) {
            set_will_sleep(msg->payload);
        } else if (msg->msghdr->commandID == DID_SLEEP_NOTIFY_COMMAND) {
            set_did_sleep(msg->payload);
        } else if (msg->msghdr->commandID == BATTERY_VOLTAGE_STATE_CHANGE_NOTIFY_COMMAND) {
            set_motor_fault_state(msg->payload);
        } else {
            write_message(data_pipe[WRITE], msg);
        }
    }
}

/***********************************************************************************/

void init_port() {
    
    char *portname = "/dev/ttyS0";
    serial_port_fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_port_fd < 0) {
        fprintf (stderr, "error %d opening %s: %s", errno, portname, strerror (errno));
        return;
    }

    set_interface_attribs (serial_port_fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (serial_port_fd, 0); 

    int err = pipe(data_pipe);
    pthread_create(&input_thread, NULL, read_thread, NULL);

    sequence_number = 1;
    logging_level = NOLOGGING;
    port_configured = 1;
}       

void close_port() {
    reading = 0;
    sleep(1);
    pthread_cancel(input_thread);
}

void set_logging_level(int level) {
    logging_level = level;
}

void log_flags(struct header * headr) {
    printf("%1d.......: %s\n", headr->flags.flag_bits.is_response, 
                               (headr->flags.flag_bits.is_response==0?"no response":"is a response"));
    printf(".%1d......: %s\n", headr->flags.flag_bits.response_requested, 
                               (headr->flags.flag_bits.response_requested==0?"no response requested":"response requested"));
    printf("..%1d.....: %s\n", headr->flags.flag_bits.response_requested_if_error, 
                               (headr->flags.flag_bits.response_requested_if_error==0?"no error response requested":"error response requested"));
    printf("...%1d....: %s\n", headr->flags.flag_bits.is_activity,
                               (headr->flags.flag_bits.is_activity==0?"is not an activity":"is an activity"));
    printf("....%1d...: %s\n", headr->flags.flag_bits.has_target, 
                               (headr->flags.flag_bits.has_target==0?"no target":"has target"));
    printf(".....%1d..: %s\n", headr->flags.flag_bits.has_source, 
                               (headr->flags.flag_bits.has_source==0?"no source":"has source"));
    printf("......0.: %s\n", "UNUSED");
    printf(".......%1d: %s\n", headr->flags.flag_bits.has_more_flags, 
                               (headr->flags.flag_bits.has_more_flags==0?"no more flags":"has more flags"));
}

void log_packet(struct message * msg) {
    u_int8_t bite;

    log_flags(msg->msghdr);
    printf("Target: %02x, Source: %02x\n", msg->msghdr->targetID, msg->msghdr->sourceID);
    printf("Command ID: %02x, Device ID: %02x\n", msg->msghdr->commandID, msg->msghdr->deviceID);
    printf("Sequence #: %02x\n", msg->msghdr->sequence_num);
    if (msg->msghdr->flags.flag_bits.is_response == 1) 
        printf("Error code: %d (%s)\n", msg->msghdr->error_code, errmsg(msg->msghdr->error_code));
    printf("Payload: {");
    u_int8_t * pl = msg->payload;
    for (int i=0; i < msg->loadlength; i++) {
        bite = *pl;
        printf("%02x ", bite);
        pl++;
    }
    printf("}\n");
}

int data_waiting(int fd) {
    int count;
    ioctl(fd, FIONREAD, &count);
    if (logging_level >= VERBOSE) 
        printf("*** Bytes waiting = %d\n", count);
    return count;
}

u_int8_t readbyte(int fd, int log) {
    u_int8_t bite;
    int bytes;

    bytes = read(fd, &bite, 1);
    if (bytes == 0) {
        if (log && logging_level >= VERBOSE) printf("*** NO DATA ***\n");
        return -1;
    }
    if (log && logging_level >= BYTESINFO) 
        if (fd == serial_port_fd) {
            printf("s%02x ", bite);
        } else {
            printf("p%02x ",bite);
        }
    if (bite == ESCAPE) {
        read(fd, &bite, 1);
        if (log && logging_level >= BYTESINFO) printf("%02x/", bite);
        bite = bite | 0x88;
        if (log && logging_level >= BYTESINFO) printf("%02x ", bite);
    }
    return bite;
}

void writebyte(int fd, u_int8_t bite) {
    u_int8_t first;
    if (bite == MESSAGE_START && fd == serial_port_fd) {
        first = ESCAPE;
        write(fd, &first, 1);
        if (logging_level >= BYTESINFO) printf("%02x ", first);
        bite = ESC_SOP;
    } else if (bite == MESSAGE_END && fd == serial_port_fd) {
        first = ESCAPE;
        write(fd, &first, 1);
        if (logging_level >= BYTESINFO) printf("%02x ", first);
        bite = ESC_EOP;
    }
    write(fd, &bite, 1);
    if (logging_level >= BYTESINFO) 
        if (fd == serial_port_fd) {
            printf("S%02x ", bite);
        } else {
            printf("P%02x ", bite);
        } 
}

struct message * read_message(int fd) {

    u_int8_t bite;
    int checksum = 0;
    u_int8_t buffer[256];

    struct message * msg = (struct message*) malloc(sizeof(struct message));
    struct header * headr = (struct header *) malloc(sizeof (struct header));
    msg->msghdr = headr;

    /* Start the message */
    if (logging_level >= BYTESINFO) {
        if (fd == serial_port_fd) 
            printf("READ SERIAL: [");
        else
            printf("READ PIPE: [");
    }
    bite = readbyte(fd,TRUE);
    if (bite == -1) return NULL;
    while (bite != MESSAGE_START) {
        if (logging_level >= BYTESINFO) printf("-");
        bite = readbyte(fd,TRUE);
    }

    /* Flags */
    bite = readbyte(fd,TRUE);
    msg->msghdr->flags.allbits = bite;
    checksum += bite;

    /* Rest of the header */
    if (msg->msghdr->flags.flag_bits.has_target == 1) {
        bite = readbyte(fd,TRUE);
        msg->msghdr->targetID.wholeID = bite;;
        checksum += bite;
    }

    if (msg->msghdr->flags.flag_bits.has_source == 1) {
        bite = readbyte(fd,TRUE);
        msg->msghdr->sourceID.wholeID = bite;;
        checksum += bite;
    }

    bite = readbyte(fd,TRUE);
    msg->msghdr->deviceID = bite;
    checksum += bite;
 
    bite = readbyte(fd,TRUE);
    msg->msghdr->commandID = bite;
    checksum += bite;

    bite = readbyte(fd, TRUE);
    msg->msghdr->sequence_num = bite;
    checksum += bite;

    /* error code byte */
    if (msg->msghdr->flags.flag_bits.is_response) {
        bite = readbyte(fd,TRUE);
        msg->msghdr->error_code = bite;
        checksum += bite;
    }

    /* And now the payload */
    u_int8_t recv_length = 0;
    if (logging_level >= BYTESINFO) printf("{ ");
    bite = readbyte(fd,FALSE);
    while (bite != MESSAGE_END) {
        buffer[recv_length] = bite;
        checksum += bite;
        recv_length++;
        bite = readbyte(fd,FALSE);
        //if (bite != MESSAGE_END && logging_level >= BYTESINFO) printf("%02x ", bite);
    }
    recv_length--;   // account for the received checksum
    msg->loadlength = recv_length;
    u_int8_t * pl = (u_int8_t *) malloc(sizeof(u_int8_t)*recv_length);
    msg->payload = pl;
    for (int i=0; i<recv_length; i++) {
        *(pl++) = buffer[i];
        if (logging_level >= BYTESINFO) 
            if (fd == serial_port_fd) {
                printf("s%02x ", buffer[i]);
            } else {
                printf("p%02x ", buffer[i]);
            }
    }
    if (logging_level >= BYTESINFO) 
        if (fd == serial_port_fd) {
            printf("} s%02x s%02x]\n", buffer[recv_length], bite);
        } else {
            printf("} p%02x p%02x]\n", buffer[recv_length], bite);
        }
    
    /* Checksum */
    checksum = checksum & 0xFF ^ 0xFF;
    if (bite != checksum) {
    }
    
    /******* Get verbose for logging *********/
    if (logging_level >= VERBOSE) log_packet(msg);

    // if ((msg->msghdr->deviceID == SENSOR_DEVICE_ID) &&
    //    (msg->msghdr->commandID == GYRO_MAX_NOTIFY || 
    //     msg->msghdr->commandID == ROBOT_TO_ROBOT_INFRARED_MESSAGE_RECEIVED_NOTIFY ||
    //     msg->msghdr->commandID == COLOR_DETECTION_NOTIFY ||
    //     msg->msghdr->commandID == STREAMING_SERVICE_DATA_NOTIFY ||
    //     msg->msghdr->commandID == MOTOR_THERMAL_PROTECTION_STATUS_NOTIFY)) {
        
    //     // process the data from the notify
    //     u_int8_t * color_data, *sensor_data, *temperature_data;
    //     if (msg->msghdr->commandID == GYRO_MAX_NOTIFY) {
    //         gyro_flags = *(msg->payload);
    //     } else if (msg->msghdr->commandID == ROBOT_TO_ROBOT_INFRARED_MESSAGE_RECEIVED_NOTIFY) {
    //         infrared_code = *(msg->payload);
    //     } else if (msg->msghdr->commandID == COLOR_DETECTION_NOTIFY) {
    //         color_data = msg->payload;
    //     } else if (msg->msghdr->commandID == STREAMING_SERVICE_DATA_NOTIFY) {
    //         sensor_data = msg->payload;
    //         set_sensor_data(sensor_data);
    //     } else if (msg->msghdr->commandID == MOTOR_THERMAL_PROTECTION_STATUS_NOTIFY) {
    //         temperature_data = msg->payload;
    //     }

    //     // recurse to get another data packet
    //     printf("Recursing\n");
    //     return read_message(data_pipe[READ]);
    //     //return read_message(serial_port_fd);

    // } else
        return msg;
}

void write_message(int fd, struct message * msg) { 

    u_int8_t bite;
    int checksum = 0;

    /* Start the message */
    bite = MESSAGE_START;
    write(fd, &bite, 1);
    if (logging_level >= BYTESINFO) {
        if (fd == serial_port_fd) 
            printf("WRITE SERIAL: [S%02x ", bite);
        else
            printf("WRITE PIPE: [P%02x ", bite);
    }

    /* Flags */
    bite = msg->msghdr->flags.allbits;
    writebyte(fd,bite);
    checksum += bite;

    /* Rest of the header */
    if (msg->msghdr->flags.flag_bits.has_target == 1) {
        bite = msg->msghdr->targetID.wholeID;
        writebyte(fd,bite);
        checksum += bite;
    }

    if (msg->msghdr->flags.flag_bits.has_source == 1) {
        bite = msg->msghdr->sourceID.wholeID;
        writebyte(fd,bite);
        checksum += bite;
    }

    bite = msg->msghdr->deviceID;
    writebyte(fd,bite);
    checksum += bite;
 
    bite = msg->msghdr->commandID;
    writebyte(fd,bite);
    checksum += bite;

    bite = msg->msghdr->sequence_num;
    writebyte(fd,bite);
    checksum += bite;

    if (fd != serial_port_fd) {
        bite = msg->msghdr->error_code;
        writebyte(fd,bite);
        checksum += bite;
    }

    /* And now the payload */
    if (msg->loadlength > 0) {
        u_int8_t * pl = msg->payload;
        if (logging_level >= BYTESINFO) printf("{ ");
        for (int i=0; i < msg->loadlength; i++) {
            bite = *pl;
            writebyte(fd,bite);
            checksum += bite;
            pl++;
        }
        if (logging_level >= BYTESINFO) printf("} ");
    }

    /* Checksum */
    bite = checksum & 0xFF ^ 0xFF;
    writebyte(fd,bite);
    
    /* Message ends */
    bite = MESSAGE_END;
    write(fd, &bite, 1);
    if (logging_level >= BYTESINFO) 
        if (fd == serial_port_fd) {
            printf("S%02x]\n", bite);
        } else {
            printf("P%02x]\n", bite);
        }

    if (logging_level >= VERBOSE) log_packet(msg);
}

void messageSend(u_int8_t cid, u_int8_t did, 
                 u_int8_t source, u_int8_t target, 
                 u_int8_t * payload, u_int8_t payload_length) {

    
    if (! port_configured) {
        printf("*** ERROR: serial port not configured.  Did you call init_port()?\n");
        return;
    }

    /* Build the header */
    struct header * headr = (struct header *) malloc(sizeof (struct header));

    /* Flags */
    headr->flags.allbits = 0;
    headr->flags.flag_bits.is_activity = 1;
    headr->flags.flag_bits.has_target = (target > 0 ? 1 : 0);
    headr->flags.flag_bits.has_source = (source > 0 ? 1 : 0);
    
    /* Rest of the header */
    headr->targetID.wholeID = target;
    headr->sourceID.wholeID = source;
    headr->deviceID = did;
    headr->commandID = cid;
    headr->sequence_num = sequence_number++;

    /* Build the message */
    struct message * msg = (struct message*) malloc(sizeof(struct message));

    msg->msghdr = headr;
    msg->payload = payload;
    msg->loadlength = payload_length;

    /* Write it! */
    write_message(serial_port_fd, msg);
}

u_int8_t * messageSendAndRecv(u_int8_t cid, u_int8_t did, 
                              u_int8_t source, u_int8_t target, 
                              u_int8_t * payload, u_int8_t payload_length, u_int8_t recv_length) {


    if (! port_configured) {
        printf("*** ERROR: serial port not configured.  Did you call init_port()?\n");
        return NULL;
    }

     /* Build the header */
    struct header * headr = (struct header *) malloc(sizeof (struct header));

    /* Flags */
    headr->flags.allbits = 0;
    headr->flags.flag_bits.is_activity = 1;
    headr->flags.flag_bits.response_requested = 1;
    headr->flags.flag_bits.has_target = (target > 0 ? 1 : 0);
    headr->flags.flag_bits.has_source = (source > 0 ? 1 : 0);
    
    /* Rest of the header */
    headr->targetID.wholeID = target;
    headr->sourceID.wholeID = source;
    headr->deviceID = did;
    headr->commandID = cid;
    headr->sequence_num = sequence_number++;

    /* Build the message */
    struct message * msg = (struct message*) malloc(sizeof(struct message));

    msg->msghdr = headr;
    msg->payload = payload;
    msg->loadlength = payload_length;

    /* Write it! */
    write_message(serial_port_fd, msg);

    /* and read it! */
    struct message * response;
    response = read_message(data_pipe[READ]);
    //response = read_message(serial_port_fd);

    return response->payload;
}

u_int8_t * messageRecv() {

    if (! port_configured) {
        printf("*** ERROR: serial port not configured.  Did you call init_port()?\n");
        return NULL;
    }
    
    struct message * response = read_message(data_pipe[READ]);
    //struct message * response = read_message(serial_port_fd);
    return response->payload;
}

char * errmsg(int error_code) {

    switch (error_code) {
        case ERROR_SUCCESS:
            return "No errors";
        case ERROR_BAD_DID:
            return "Bad device ID";
        case ERROR_BAD_CID:
            return "Bad command ID";
        case ERROR_NOT_YES_IMPLEMENTED:
            return "Not implemented";
        case ERROR_RESTRICTED:
            return "Restricted";
        case ERROR_BAD_DATA_LENGTH:
            return "Bad data length";
        case ERROR_FAILED:
            return "Failed";
        case ERROR_BAD_DATA_VALUE:
            return "Bad data value";
        case ERROR_BUSY:
            return "Busy";
        case ERROR_BAD_TID:
            return "Bad target ID";
        case ERROR_TARGET_UNAVAILABLE:
            return "Target unavailable";
        default:
            return "Bad error code";
    }
}
