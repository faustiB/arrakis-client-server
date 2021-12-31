#include "ioscreen.h"

/* ********************************************************************
 *
 * @Nombre : IOSCREEN_readUntilIntro
 * @Def : Función para leer hasta un intro.
 *
 ********************************************************************* */
char * IOSCREEN_readUntilIntro(int fd, char caracter, int i) {
    char * buffer = (char * ) malloc(1 * sizeof(char));

    while (caracter != '\n' || i == 0) {
        read(fd, & caracter, sizeof(char));
        buffer = (char * ) realloc(buffer, i + 1);
        buffer[i] = caracter;

        if (caracter == '\n' || caracter == '\0') {
            buffer[i] = '\0';
            return buffer;
        }
        i++;
    }

    return buffer;
}

/* ********************************************************************
 *
 * @Nombre : IOSCREEN_readDelimiter
 * @Def : Función para leer una linea
 *
 ********************************************************************* */
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

/* ********************************************************************
 *
 * @Nombre : IOSCREEN_read_until
 * @Def : Función para leer una linea
 *
 ********************************************************************* */
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

/* ********************************************************************
 *
 * @Nombre : IOSCREEN_isEmpty
 * @Def : Función revisar si un string es todo espacios vacios.
 *
 ********************************************************************* */
int IOSCREEN_isEmpty(const char *string) {
    while (*string != '\0') {
        if (!isspace((unsigned char)*string))
            return 0;
        string++;
    }
    return 1;
}