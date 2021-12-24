#ifndef _ATREIDES_H
#define _ATREIDES_H
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
#include <pthread.h>

#include "frame_config.h"

#define printF(x) write(1, x, strlen(x))

typedef struct {
    char *ip;
    int port;
    char *directory;
} ConfigAtreides;

typedef struct {
    int id;
    char *username;
    char *postal_code;
    int file_descriptor;
    pthread_t thread;
} User;
/*
typedef struct {
    char origin[15];
    char type;
    char data[240];
} Frame;
*/

#endif //_ATREIDES_H
