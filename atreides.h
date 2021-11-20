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

#define printF(x) write(1, x, strlen(x))

typedef struct {
    char *ip;
    int port;
    char *directory;
} Config;

typedef struct {
    int id;
    //char *username;
    char username[20];
    int postal_code;
} User;

#endif //_ATREIDES_H
