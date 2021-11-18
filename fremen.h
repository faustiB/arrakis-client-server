#ifndef FREMEN_FREMEN_H
#define FREMEN_FREMEN_H
#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>

//Define
#define printF(x) write(1, x, strlen(x))

typedef struct {
    int seconds_to_clean;
    char *ip;
    int port;
    char *directory;
} Config;


char *readUntilIntro(int fd, char caracter, int i);
Config fillConfiguration (Config c, int fd);
int promptChoice ();
int checkNumberOfWords (char *command, int words);

#endif //FREMEN_FREMEN_H
