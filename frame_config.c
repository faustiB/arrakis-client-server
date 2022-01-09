#include "frame_config.h"

/* ********************************************************************
 *
 * @Nombre : FRAME_CONFIG_receiveFrame
 * @Def : Rececpción de trama
 *
 ********************************************************************* */
Frame FRAME_CONFIG_receiveFrame(int fd) {
    int i;
    unsigned char frame_read[256];
    Frame frame;

    memset(frame.origin, 0, sizeof(frame.origin));
    memset(frame.data, 0, sizeof(frame.data));

    read(fd, frame_read, sizeof(char) * 256);

    i = 0;
    while (i < 15) {
        frame.origin[i] = frame_read[i];
        i++;
    }

    frame.type = frame_read[15];

    i = 16;
    while (i < 256) {
        frame.data[i - 16] = frame_read[i];
        i++;
    }

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FRAME_CONFIG_generateFrame
 * @Def : ceación de una trama de Fremen
 *
 ********************************************************************* */
char * FRAME_CONFIG_generateFrame(int origin) {
    char * frame;
    int i = 0;

    frame = (char * ) malloc(sizeof(char) * 256);
    memset(frame, 0, 256 * sizeof(char));

    if (origin == 1) {
        sprintf(frame, "FREMEN");
    } else if (origin == 2){
        sprintf(frame, "ATREIDES");
    }

    for (i = strlen(frame); i < 15; i++) {
        frame[i] = '\0';
    }

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FRAME_CONFIG_generateCustomFrame
 * @Def : ceación de una trama customizada.
 *
 ********************************************************************* */
char * FRAME_CONFIG_generateCustomFrame(int origin, char type, int isOk) {
    char * frame, * aux = NULL;
    int i = 0;
    unsigned int j = 0;

    frame = (char * ) malloc(sizeof(char) * 256);
    aux = (char * ) malloc(sizeof(char) * 40);

    memset(frame, 0, 256 * sizeof(char));

    if (origin == 1) {
        sprintf(frame, "FREMEN");
    } else if (origin == 2){
        sprintf(frame, "ATREIDES");
    }

    for (i = strlen(frame); i < 15; i++) {
        frame[i] = '\0';
    }

    frame[15] = type;

    if (isOk == 0) {
        sprintf(aux, "IMATGE OK");
    } else if ( isOk == 1 ) {
        sprintf(aux, "IMATGE KO");
    } else {
        sprintf(aux, "FILE NOT FOUND");
    }

    for (i = 16; aux[i - 16] != '\0'; i++) {
        frame[i] = aux[i - 16];
    }

    for (j = i; j < 256; j++) {
        frame[j] = '\0';
    }

    free(aux);
    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FRAME_CONFIG__getMD5
 * @Def : Obtención md5
 *
 ********************************************************************* */
char * FRAME_CONFIG_getMD5(char * file) {
    int link[2];
    pid_t pid;

    if (pipe(link) == -1)
        perror("pipe");

    if ((pid = fork()) == -1)
        perror("fork");

    if (pid == 0) {

        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execl("/bin/md5sum", "md5sum", file, (char * ) 0);
        perror("execl");

    } else {
        wait(NULL);
        close(link[1]);

        char * md5_out = (char * ) malloc(256* sizeof(char));
        read(link[0], md5_out, 256);
        strtok(md5_out, " ");

        close(link[0]);

        return md5_out;
    }
    return NULL;
}
