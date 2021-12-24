#ifndef _FRAME_CONFIG_H
#define _FRAME_CONFIG_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
//#include "fremen.h"


typedef struct {
    char origin[15];
    char type;
    char data[240];
} Frame;



char * FRAME_CONFIG_generateFrame(int origin);
Frame FRAME_CONFIG_receiveFrame(int fd);

#endif //_FRAME_CONFIG_H
