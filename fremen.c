
#include "fremen.h"
#include "ioscreen.h"


void RsiControlC(void);
ConfigFremen configuration;
int socket_fd, user_id, control_login;
char * user_name;

/* ********************************************************************
 *
 * @Nombre : RsiControlC
 * @Def : Rutina asociada a la interrupción Control+C.
 *
 ********************************************************************* */
void RsiControlC(void) {

    //Printamos el mensaje de adiós al sistema
    printF("\n\nAturant Fremen... \n\n");

    free(configuration.ip);
    free(configuration.directory);
    free(user_name);
    
    if (socket_fd == -1) {
        socket_fd = 0;
    }

    close(socket_fd);

    //Terminamos el programa enviándonos a nosotros mismos el signal de SIGINT
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_fillConfiguration
 * @Def : Función para leer el fichero de configuración, y devolverlo en nuestro struct.
 *
 ********************************************************************* */
ConfigFremen FREMEN_fillConfiguration(char * argv) {
    char caracter = ' ', * cadena = NULL;
    int i = 0, fd;
    ConfigFremen c;

    //Apertura del fichero
    fd = open(argv, O_RDONLY);

    if (fd < 0) {
        printF("Fitxer de configuració erroni\n");
        raise(SIGINT);

    } else {
        cadena = IOSCREEN_readUntilIntro(fd, caracter, i);
        c.seconds_to_clean = atoi(cadena);
        free(cadena);

        c.ip = IOSCREEN_readUntilIntro(fd, caracter, i);

        cadena = IOSCREEN_readUntilIntro(fd, caracter, i);
        c.port = atoi(cadena);
        free(cadena);

        c.directory = IOSCREEN_readUntilIntro(fd, caracter, i);
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

int configSocket(ConfigFremen config, char * command, char * command_lower, char ** command_array) {

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
        socket_fd = -1;
        FREMEN_freeMemory(command, command_lower, command_array);
    }

    return socket_fd;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_login
 * @Def : función de login
 *
 ********************************************************************* */

void FREMEN_login(ConfigFremen configuration, char * command, char * command_lower, char ** command_array) {
    socket_fd = configSocket(configuration, command, command_lower, command_array);
    if (socket_fd < 1) {
        printF("ERROR: no se ha podido conectar el socket\n");
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
 * @Nombre : FREMEN_generateFrameLogin
 * @Def : ceación y generación de la tramapara login.
 *
 ********************************************************************* */
char * FREMEN_generateFrameLogin(char * frame, char type, char * name, char * zipCode) {
    int i = 0, j = 0;

    char * buffer;

    frame[15] = type;

    asprintf( & buffer, "%s*%s", name, zipCode);

    for (i = 16; buffer[i - 16] != '\0'; i++) {
        frame[i] = buffer[i - 16];
    }

    for (j = i; j < 256; j++) {
        frame[j] = '\0';
    }

    free(buffer);
    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_generateFrameLogout
 * @Def : ceación y generación de la trama para logout.
 *
 ********************************************************************* */
char * FREMEN_generateFrameLogout(char * frame, char type) {
    int i = 0, j = 0;

    char * buffer, id_str[3];
    frame[15] = type;

    snprintf(id_str, 3, "%d", user_id);

    asprintf( & buffer, "%s*%s", user_name, id_str);

    for (i = 16; buffer[i - 16] != '\0'; i++) {
        frame[i] = buffer[i - 16];
    }

    for (j = i; j < 256; j++) {
        frame[j] = '\0';
    }

    free(buffer);
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

    char * buffer, id_str[3];

    frame[15] = type;

    snprintf(id_str, 3, "%d", user_id);

    asprintf( & buffer, "%s*%s*%s", user_name, id_str, zipCode);

    for (i = 16; buffer[i - 16] != '\0'; i++) {
        frame[i] = buffer[i - 16];
    }

    for (j = i; j < 256; j++) {
        frame[j] = '\0';
    }

    free(buffer);

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : FREMEN_showSearchReceived
 * @Def : Función para tratar el comando que se introduce
 *
 ********************************************************************* */

void FREMEN_showSearchReceived(char data[240], char * postal_code) {

    int i, k, num_searched_users = 0;
    char * num_searched_users_str, * name, * id_user, cadena[100];

    i = 0;
    num_searched_users_str = (char * ) malloc(1 * sizeof(char));
    while (data[i] != '*') {
        num_searched_users_str[i] = data[i];
        num_searched_users_str = (char * ) realloc(num_searched_users_str, i + 2);
        i++;
    }
    num_searched_users_str[i] = '\0';
    num_searched_users = atoi(num_searched_users_str);

    i++;

    sprintf(cadena, "\nHi ha %s persones humanes a %s\n", num_searched_users_str, postal_code);
    write(STDOUT_FILENO, cadena, strlen(cadena));

    for (int j = 0; j < num_searched_users; j++) {
        k = 0;
        name = (char * ) malloc(1 * sizeof(char));

        while (data[i] != '*') {
            name[k] = data[i];
            name = (char * ) realloc(name, k + 2);
            i++;
            k++;
        }

        name[k] = '\0';
        i++;

        id_user = (char * ) malloc(1 * sizeof(char));
        k = 0;

        while (data[i] != '*' && data[i] != '\0') {
            id_user[k] = data[i];
            id_user = (char * ) realloc(id_user, k + 2);
            i++;
            k++;
        }
        id_user[k] = '\0';

        i++;

        sprintf(cadena, "%s %s\n", id_user, name);
        write(STDOUT_FILENO, cadena, strlen(cadena));

        free(name);
        free(id_user);
    }

    free(num_searched_users_str);

}

/* ********************************************************************
 *
 * @Nombre : FREMEN_promptChoice
 * @Def : Función para tratar el comando que se introduce
 *
 ********************************************************************* */
int FREMEN_promptChoice(ConfigFremen configuration) {
    char * command = NULL, * command_lower = NULL, * frame = NULL, cadena[200];
    char * ( * command_array);
    int i = 0, num_of_words = 0, isok = 0;

    //Lectura por pantalla del comando y tratado para quedarnos con una cadena
    printF("$ ");
    command = IOSCREEN_readUntilIntro(STDIN_FILENO, '\n', 0);

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
    command_lower = strdup(command_array[0]);
    for (size_t i = 0; command_lower[i] != '\0'; ++i) {
        command_lower[i] = tolower((unsigned char) command_lower[i]);
    }

    //Checkeo del número de parametros.
    isok = FREMEN_checkNumberOfWords(command_lower, num_of_words);

    //Comando custom OK
    if (isok == 0) {
        if (strcmp(command_lower, "logout") == 0) {
            if (socket_fd > 0) {

                frame = FRAME_CONFIG_generateFrame(1);
                frame = FREMEN_generateFrameLogout(frame, 'Q');
                FREMEN_sendFrame(socket_fd, frame);

                free(frame);
                close(socket_fd);
                socket_fd = 0;

                printF("\nDesconnectat d’Atreides. Dew!\n\n");
                FREMEN_freeMemory(command, command_lower, command_array);

            } else {
                printF("No puc fer logout si no estic connectat al servidor...\n");
            }

        } else if (strcmp(command_lower, "login") == 0) {
            if (socket_fd == 0) {

                frame = NULL;

                frame = FRAME_CONFIG_generateFrame(1);
                FREMEN_login(configuration, command, command_lower, command_array);

                if (socket_fd > 0) {
                    frame = FREMEN_generateFrameLogin(frame, 'C', command_array[1], command_array[2]);

                    FREMEN_sendFrame(socket_fd, frame);

                    printF("Conectado al servidor\n");

                    Frame frame_received;

                    frame_received = FRAME_CONFIG_receiveFrame(socket_fd);

                    if (frame_received.type == 'O') {
                        control_login = 1;

                        user_id = atoi(frame_received.data);
                        user_name = (char * ) malloc(sizeof(char) * strlen(command_array[1]) + 1);
                        strcpy(user_name, command_array[1]);

                        sprintf(cadena, "Benvingut %s. Tens ID: %d. \n", user_name, user_id);
                        write(STDOUT_FILENO, cadena, strlen(cadena));
                        printF("Ara estàs connectat a Atreides.\n");

                    } else if (frame_received.type == 'E') {
                        printF("Error a l'hora de fer el login. \n");
                    }
                } else {
                    printF("No puc fer login, sembla que Atreides està aturat...\n");

                    //Reiniciem el socket a 0
                    socket_fd = 0;
                }
                free(frame);
                
            } else if (socket_fd > 0){
                printF("No puc fer login si ja estic connectat al servidor amb un altre usuari...\n");
            }

        } else if (strcmp(command_lower, "search") == 0) {
            if (socket_fd > 0) {
                frame = NULL;

                frame = FRAME_CONFIG_generateFrame(1);
                frame = FREMEN_generateFrameSearch(frame, 'S', command_array[1]);
                FREMEN_sendFrame(socket_fd, frame);
                Frame frame_received;
                frame_received = FRAME_CONFIG_receiveFrame(socket_fd);

                FREMEN_showSearchReceived(frame_received.data, command_array[1]);

                free(frame);
            } else {
                printF("No es pot buscar perque no esteu loginats al servidor... \n");
            }

        } else if (strcmp(command_lower, "send") == 0) {
            //Implementar fase 3
        } else if (strcmp(command_lower, "photo") == 0) {
            //Implementar fase 3
        }

        command = NULL;
        command_lower = NULL;
        command_array = NULL;
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