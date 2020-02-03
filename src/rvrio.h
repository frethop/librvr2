#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>
#include <sys/ioctl.h> 

#include "rvrdefs.h"
#include "notify.h"

#define ESCAPE   0xab
#define ESC_ESC  0x23
#define ESC_SOP  0x05
#define ESC_EOP  0x50 

int logging_level;

void init_port();
void close_port();
void set_logging_level(int level);

struct message * read_message(int fd);
int data_waiting();
void messageSend(u_int8_t cid, u_int8_t did, 
                 u_int8_t source, u_int8_t target, 
                 u_int8_t * payload, u_int8_t payload_length); 
u_int8_t * messageSendAndRecv(u_int8_t cid, u_int8_t did, 
                              u_int8_t source, u_int8_t target, 
                              u_int8_t * payload, u_int8_t payload_length, u_int8_t recv_length); 
u_int8_t * messageRecv();
char * errmsg(int error_code);
