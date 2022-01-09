//Define Guard
#ifndef _FREMEN_H
#define _FREMEN_H
#define _GNU_SOURCE

//Llibreries del sistema
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

//Llibreries pròpies
#include "frame_config.h"

//Define
#define printF(x) write(1, x, strlen(x))

//Tipus propis
typedef struct {
    int seconds_to_clean;
    char *ip;
    int port;
    char *directory;
} ConfigFremen;

#endif //_FREMEN_H
