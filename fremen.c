#include "fremen.h"

void RsiControlC(void);
Config configuration;
int socket_fd;
char * global_name, * global_id;

/* ********************************************************************
 *
 * @Nombre : RsiControlC
 * @Def : Rutina asociada a la interrupción Control+C.
 *
 ********************************************************************* */
void RsiControlC(void) {

    //Printamos el mensaje de adiós al sistema
    printF("\n\nDesconnectat d’Atreides. Dew!\n\n");

    free(configuration.ip);
    free(configuration.directory);
    close(socket_fd);

    //Terminamos el programa enviándonos a nosotros mismos el signal de SIGINT
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_readUntilIntro
 * @Def : Función para leer hasta un intro.
 *
 ********************************************************************* */
char * FREMEN_readUntilIntro(int fd, char caracter, int i) {
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
 * @Nombre : FREMEN_fillConfiguration
 * @Def : Función para leer el fichero de configuración, y devolverlo en nuestro struct.
 *
 ********************************************************************* */
Config FREMEN_fillConfiguration(char * argv) {
    char caracter = ' ', * cadena = NULL;
    int i = 0, fd;
    Config c;

    //Apertura del fichero
    fd = open(argv, O_RDONLY);

    if (fd < 0) {
        printF("Fitxer de configuració erroni\n");
        raise(SIGINT);

    } else {
        cadena = FREMEN_readUntilIntro(fd, caracter, i);
        c.seconds_to_clean = atoi(cadena);
        free(cadena);

        c.ip = FREMEN_readUntilIntro(fd, caracter, i);

        cadena = FREMEN_readUntilIntro(fd, caracter, i);
        c.port = atoi(cadena);
        free(cadena);

        c.directory = FREMEN_readUntilIntro(fd, caracter, i);
        close(fd);
    }

    //Lectura del fichero de configuración y cierre de su file descriptor.
    return c;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_freeMemory
 * @Def : Función para liberar la memoria.
 *
 ********************************************************************* */

void FREMEN_freeMemory(char * command, char * command_lower, char ** command_array) {
    free(command);
    free(command_lower);
    free(command_array);
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_checkNumberOfWords
 * @Def : Función para comprobar el número de parametros para el comando.
 *
 ********************************************************************* */
int FREMEN_checkNumberOfWords(char * command, int words) {

    if (strcmp(command, "login") == 0) {
        if (words == 3) {
            //printF("Comanda OK.\n");
            return 0;
        } else if (words < 3) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcmp(command, "search") == 0) {
        if (words == 2) {
            //printF("Comanda OK.\n");
            return 0;
        } else if (words < 2) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcmp(command, "send") == 0) {
        if (words == 2) {
            //printF("Comanda OK.\n");
            return 0;
        } else if (words < 2) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcmp(command, "photo") == 0) {
        if (words == 2) {
            //printF("Comanda OK.\n");
            return 0;
        } else if (words < 2) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcmp(command, "logout") == 0) {
        if (words == 1) {
            return 0;
        } else if (words < 1) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    }

    //Si no es ninguno de los comandos anteriores, se devuelve 2 porque seguramente
    //sea un comando linux. Así se ejecuta en contra el sistema.
    return 2;
}

/* ********************************************************************
 *
 * @Nombre : configSocket
 * @Def : Preparación conexión atreides.
 *
 ********************************************************************* */

int configSocket(Config config, char * command, char * command_lower, char ** command_array) {

    struct sockaddr_in s_addr;
    int socket_fd;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printF("ERROR: crear socket del cliente\n");
        FREMEN_freeMemory(command, command_lower, command_array);
        raise(SIGINT);
    }

    memset( & s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(config.port);
    s_addr.sin_addr.s_addr = inet_addr(config.ip);

    if (connect(socket_fd, (void * ) & s_addr, sizeof(s_addr)) < 0) {
        printF("ERROR: connect del cliente\n");
        close(socket_fd);
        FREMEN_freeMemory(command, command_lower, command_array);
        raise(SIGINT);
    }

    return socket_fd;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_login
 * @Def : función de login
 *
 ********************************************************************* */

void FREMEN_login(Config configuration, char * command, char * command_lower, char ** command_array) {
    socket_fd = configSocket(configuration, command, command_lower, command_array);
    if (socket_fd < 1) {
        printF("ERROR: no se ha podido conectar el socket\n");
        raise(SIGINT);
    }
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_sendFrame
 * @Def : Envío de la trama
 *
 ********************************************************************* */
void FREMEN_sendFrame(int fd, char * frame) {
    write(fd, frame, 256);
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_generateFrame
 * @Def : ceación de una trama de Fremen
 *
 ********************************************************************* */
char * FREMEN_generateFrame() {
    char * frame;
    int i = 0;

    frame = (char * ) malloc(sizeof(char) * 256);

    sprintf(frame, "FREMEN");

    for (i = strlen(frame); i < 15; i++) {
        frame[i] = '\0';
    }

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_generateFrameLogin
 * @Def : ceación y generación de la tramapara login.
 *
 ********************************************************************* */
char * FREMEN_generateFrameLogin(char * frame, char type, char * name, char * zipCode) {
    int i = 0, j = 0;

    char * buffer;

    frame[15] = type;

    asprintf(&buffer, "%s*%s", name , zipCode);

    for (i = 16; buffer[i-16] != '\0'; i++) {
        frame[i] = buffer[i-16];
    }

    for(j = i; j < 256; j++){
      frame[j] = '\0';
    }

    free(buffer);
    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_generateFrameLogout
 * @Def : ceación y generación de la tramapara logout.
 *
 ********************************************************************* */
char * FREMEN_generateFrameLogout(char * frame, char type) {
    int i = 0, j = 0;

    char * buffer;
    //Cambiar char* por char
    frame[15] = type;

    asprintf(&buffer, "%s*%s", global_name , global_id);

    for (i = 16; buffer[i-16] != '\0'; i++) {
        frame[i] = buffer[i-16];
    }

    for(j = i; j < 256; j++){
      frame[j] = '\0';
    }

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_generateFrameSearch
 * @Def : ceación y generación de la tramapara logout.
 *
 ********************************************************************* */
char * FREMEN_generateFrameSearch(char * frame, char type, char * zipCode) {
    int i = 0, j = 0;

    char * buffer;
    //Cambiar char* por char
    frame[15] = type;

    asprintf(&buffer, "%s*%s*%s", global_name , global_id, zipCode);

    for (i = 16; buffer[i-16] != '\0'; i++) {
        frame[i] = buffer[i-16];
    }

    for(j = i; j < 256; j++){
      frame[j] = '\0';
    }

    free(buffer);

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_promptChoice
 * @Def : Función para tratar el comando que se introduce
 *
 ********************************************************************* */
int FREMEN_promptChoice(Config configuration) {
    char * command = NULL, * command_lower = NULL, * frame = NULL;
    char * ( * command_array), character = ' ';
    int i = 0, num_of_words = 0, isok = 0;

    //Lectura por pantalla del comando y tratado para quedarnos con una cadena
    printF("$ ");
    command = FREMEN_readUntilIntro(STDIN_FILENO, '\n', 0);

    //Intro vacío
    if (command[0] == '\0') {
        free(command);
        return 0;
    }

    //Tratamiento del comando para separar la cadena de entrada.
    command_array = (char ** ) malloc(sizeof(char * ));
    command_array[i] = strtok(command, " ");

    while (command_array[i] != NULL) {
        num_of_words++;
        command_array = realloc(command_array, (num_of_words + 1) * sizeof(char * ));
        command_array[++i] = strtok(NULL, " ");
    }

    //Tratamiento pasar cadena a minúscula
    command_lower = strdup(command);
    i = 0;
    if (num_of_words == 1) {
        character = '\0';
    } else {
        character = ' ';
    }

    for (size_t i = 0; command_lower[i] != character; ++i) {
            command_lower[i] = tolower((unsigned char) command_lower[i]);
        }

    //Checkeo del número de parametros.
    isok = FREMEN_checkNumberOfWords(command_array[0], num_of_words);

    //Comando custom OK
    if (isok == 0) {
        if (strcmp(command_array[0], "logout") == 0) {
            if (socket_fd > 0) {
                //frame = NULL;

                frame = FREMEN_generateFrame();
                frame = FREMEN_generateFrameLogout(frame, 'Q');
                FREMEN_sendFrame(socket_fd, frame);

                printf("%s\n", frame);

                free(frame);
                close(socket_fd);

                FREMEN_freeMemory(command, command_lower, command_array);

                raise(SIGINT);
            } else {
                printF("No puc fer logout si no estic connectat al servidor...\n");
            }

        } else if (strcmp(command_array[0], "login") == 0) {
            frame = NULL;

            frame = FREMEN_generateFrame();
            FREMEN_login(configuration, command, command_lower, command_array);

            frame = FREMEN_generateFrameLogin(frame, 'C', command_array[1], command_array[2]);
            FREMEN_sendFrame(socket_fd, frame);

            printF("Conectado al servidor\n");

            //Al rebre el ID des de atreides, hem de guardar com a variable global el nom i el ID, ja que
            //ho necessitem per totes les altres opcions de fremen.
            //Siguiendo en la linea de lo que comentas, se ha de hacer un read al fddel socket, para leer la respuesta.

            free(frame);

        } else if (strcmp(command_array[0], "search") == 0) {
            if (socket_fd > 0) {
                frame = NULL;

                frame = FREMEN_generateFrame();
                frame = FREMEN_generateFrameSearch(frame, 'S', command_array[1]);
                FREMEN_sendFrame(socket_fd, frame);

                printf("%s\n", frame);
                free(frame);
            } else {
                printF("No es pot buscar perque no esteu loginats al servidor... \n");
            }

        } else if (strcmp(command_array[0], "send") == 0) {
            //Implementar fase 3
        } else if (strcmp(command_array[0], "photo") == 0) {
            //Implementar fase 3
        }

        FREMEN_freeMemory(command, command_lower, command_array);
        return 1;
        //Comando custom KO, por parametros
    } else if (isok == 1) {
        FREMEN_freeMemory(command, command_lower, command_array);
        free(frame);
        return 0;

        //Cuando el  valor retornado sea 2, se abrirá un fork para ejecutar la comanda contra el sistema
    } else {
        pid_t pid;

        //Creación del fork
        if ((pid = fork()) < 0) {
            FREMEN_freeMemory(command, command_lower, command_array);
            printF("Error al crear el Fork\n");
            exit(1);
        }
        //Entrada al código del hijo.
        else if (pid == 0) {
            //Ejecución del comando +  tratamiento si ha ido mal
            if (execvp(command_array[0], command_array) < 0) {
                FREMEN_freeMemory(command, command_lower, command_array);
                free(frame);
                printF("Error al executar la comanda!\n");
            }
            //Código del padre
        } else {
            //Espera a la ejecución del comando.
            wait(NULL);

            //liberacion de memoria
            FREMEN_freeMemory(command, command_lower, command_array);
        }
        return 0;
    }
}

/* ********************************************************************
 *
 * @Nombre : Main
 * @Def : Función principal.
 *
 ********************************************************************* */
int main(int argc, char ** argv) {

    int command = 0;

    //Check de argumentos de entrada
    if (argc != 2) {
        printF("Error, falta el nom de fitxer de configuracio.\n");
        return -1;
    }

    //Assignación del signal de CtrlC a nuestra función
    signal(SIGINT, (void * ) RsiControlC);
    //Lectura del fichero de configuración y cierre de su file descriptor.
    configuration = FREMEN_fillConfiguration(argv[1]);

    //Inicialización de fremen.
    printF("Benvingut a Fremen\n");
    while (command == 0 || command == 1) {
        command = FREMEN_promptChoice(configuration);
    }

    //Exit de fremen con la palabra logout.
    raise(SIGINT);

    return 0;
}
