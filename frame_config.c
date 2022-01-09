/***********************************************
*
* @Proposit:
* @Autor/s: Faozi Bouybaouene Gadrouz i Guillem Miró Sierra
* @Data ultima modificacio: 09/01/2022
*
************************************************/

#include "frame_config.h"

/***********************************************
*
* @Nom: FRAME_CONFIG_receiveFrame
* @Finalitat: Funció creada per a rebre un frame i desar-lo en una estructura de tipus Frame (trama).
* @Parametres: int fd: file descriptor del socket.
* @Retorn: objecte del tipus Frame amb les dades.
*
************************************************/
Frame FRAME_CONFIG_receiveFrame(int fd) {
    int i;
    unsigned char frame_read[256];
    Frame frame;

    //inicialitzem les dues cadenes amb zeros.
    memset(frame.origin, 0, sizeof(frame.origin));
    memset(frame.data, 0, sizeof(frame.data));

    //llegim el frame d'arribada
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

/***********************************************
*
* @Nom: FRAME_CONFIG_generateFrame
* @Finalitat: Generació d'una trama buida amb el codi d'on prové.
* @Parametres: int origin: 1 per fremen, 2 per atreides.
* @Retorn: cadena de text amb la trama.
*
************************************************/
char * FRAME_CONFIG_generateFrame(int origin) {
    char * frame;
    int i = 0;

    //aloquem memòria per al frame i l'inicialitzem a zeros.
    frame = (char * ) malloc(sizeof(char) * 256);
    memset(frame, 0, 256 * sizeof(char));

    //assignem l'origen i el fiquem a la cadena
    if (origin == 1) {
        sprintf(frame, "FREMEN");
    } else if (origin == 2) {
        sprintf(frame, "ATREIDES");
    }

    //omplim amb \0
    for (i = strlen(frame); i < 15; i++) {
        frame[i] = '\0';
    }

    return frame;
}

/***********************************************
*
* @Nom: FRAME_CONFIG_generateCustomFrame
* @Finalitat: Generació d'una trama sencera per als mètodes photo i send.
* @Parametres: int origin: 1 fremen, 2 atreides, char type: tipus de trama, int isOk: 1 ok, 2 ko, 3 not found.
* @Retorn: trama llesta per enviar
*
************************************************/
char * FRAME_CONFIG_generateCustomFrame(int origin, char type, int isOk) {
    char * frame, * aux = NULL;
    int i = 0;
    unsigned int j = 0;

    //aloquem memòria per les cadenes
    frame = (char * ) malloc(sizeof(char) * 256);
    aux = (char * ) malloc(sizeof(char) * 40);

    //inicialitzem a zeros el frame.
    memset(frame, 0, 256 * sizeof(char));

    //assignem l'origen i el fiquem a la cadena
    if (origin == 1) {
        sprintf(frame, "FREMEN");
    } else if (origin == 2) {
        sprintf(frame, "ATREIDES");
    }

    for (i = strlen(frame); i < 15; i++) {
        frame[i] = '\0';
    }

    //assignem el caràcter de la trama.
    frame[15] = type;

    if (isOk == 0) {
        sprintf(aux, "IMATGE OK");
    } else if (isOk == 1) {
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

/***********************************************
*
* @Nom: FRAME_CONFIG_getMD5
* @Finalitat: Generar el codi md5 i emmagatzemar-lo a una cadena.
* @Parametres: char * file: nom del fitxer.
* @Retorn: cadena amb el codi md5.
*
************************************************/
char * FRAME_CONFIG_getMD5(char * file) {
    int link[2];
    pid_t pid;

    if (pipe(link) == -1)
        perror("pipe");

    if ((pid = fork()) == -1)
        perror("fork");

    //codi del fill
    if (pid == 0) {

        //generem el codi del md5sum amb execl i el passem a stdout
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execl("/bin/md5sum", "md5sum", file, (char * ) 0);
        perror("execl");

    } else {
        wait(NULL);
        close(link[1]);

        //aloquem memòria per la cadena
        char * md5_out = (char * ) malloc(256 * sizeof(char));
        //llegim pel pipe la cadena
        read(link[0], md5_out, 256);
        strtok(md5_out, " ");

        close(link[0]);

        return md5_out;
    }
    return NULL;
}