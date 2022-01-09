/***********************************************
*
* @Proposit:
* @Autor/s: Faozi Bouybaouene Gadrouz i Guillem Miró Sierra
* @Data ultima modificacio: 09/01/2022
*
************************************************/

#include "ioscreen.h"

/***********************************************
*
* @Nom: IOSCREEN_readUntilIntro
* @Finalitat: Funció que llegeix una cadena fins a un \n.
* @Parametres: int fd: file descriptor d'escriptura,
 * char caracter: caracter per escrirue , int i.
* @Retorn: cadena llegida.
*
************************************************/
char * IOSCREEN_readUntilIntro(int fd, char caracter, int i) {
    char * buffer = NULL;

    while (caracter != '\n' || i == 0) {
        read(fd, & caracter, sizeof(char));

        //Aloquem memòria només si el buffer és NULL.
        if (buffer == NULL) {
            buffer = (char * ) malloc(1 * sizeof(char));
        } else {
            buffer = (char * ) realloc(buffer, i + 1);
        }
        buffer[i] = caracter;

        //Si arribem a un enter o un \0 tallem l'execució.
        if (caracter == '\n' || caracter == '\0') {
            buffer[i] = '\0';
            return buffer;
        }
        i++;
    }

    return buffer;
}

/***********************************************
*
* @Nom: IOSCREEN_readDelimiter
* @Finalitat: Funció que llegeix una cadena fins a cert delimitador.
* @Parametres: int fd: file descriptor d'escriptura, char delimiter.
* @Retorn: cadena llegida.
*
************************************************/
char * IOSCREEN_readDelimiter(int fd, char delimiter) {

    char * msg = malloc(1);
    char current;
    int i = 0;
    int len = 0;
    while ((len += read(fd, & current, 1)) > 0) {

        msg[i] = current;
        msg = (char * ) realloc(msg, ++i + 1);
        if (current == delimiter)
            break;
    }
    msg[i - 1] = '\0';

    return msg;
}

/***********************************************
*
* @Nom: IOSCREEN_read_until
* @Finalitat: Funció que llegeix una cadena fins a cert delimitador.
* @Parametres: int fd: file descriptor d'escriptura, char end: delimiter.
* @Retorn:  cadena llegida.
*
************************************************/
char * IOSCREEN_read_until(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char * string = (char * ) malloc(sizeof(char));

    while (1) {
        size = read(fd, & c, sizeof(char));

        if (c != end && size > 0) {
            string = (char * ) realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            break;
        }
    }

    string[i] = '\0';
    return string;
}

/***********************************************
*
* @Nom: IOSCREEN_isEmpty
* @Finalitat: Revisa que una cadena no estigui plena d'espais només.
* @Parametres: const char * string: cadena d'entrada
* @Retorn: 0 per no són espais, 1 són espais.
*
************************************************/
int IOSCREEN_isEmpty(const char * string) {
    while ( * string != '\0') {
        if (!isspace((unsigned char) * string))
            return 0;
        string++;
    }
    return 1;
}