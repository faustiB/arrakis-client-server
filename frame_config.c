#include "frame_config.h"

/* ********************************************************************
 *
 * @Nombre : FRAME_CONFIG_receiveFrame
 * @Def : Rececpción de trama
 *
 ********************************************************************* */
Frame FRAME_CONFIG_receiveFrame(int fd) {
    int i;
    char frame_read[256];
    Frame frame;

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

    if (origin == 1)
    {

        sprintf(frame, "FREMEN");


    } else if (origin == 2){

        sprintf(frame, "ATREIDES");

    }

    for (i = strlen(frame); i < 15; i++) {
        frame[i] = '\0';
    }

    return frame;
}
