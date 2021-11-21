#ifndef _FREMEN_H
#define _FREMEN_H
#define _GNU_SOURCE


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

//Define
#define printF(x) write(1, x, strlen(x))

typedef struct {
    int seconds_to_clean;
    char *ip;
    int port;
    char *directory;
} Config;

typedef struct {
    char origen[15];
    char tipus;
    char data[240];
} Trama;


#endif //_FREMEN_H
