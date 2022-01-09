/***********************************************
*
* @Proposit:
* @Autor/s: Faozi Bouybaouene Gadrouz i Guillem Miró Sierra
* @Data ultima modificacio: 09/01/2022
*
************************************************/

//Llibreries pròpies
#include "fremen.h"
#include "ioscreen.h"

void RsiControlC(void);

//Variables globals
ConfigFremen configuration;
int socket_fd, user_id, control_login;
char * user_name;

/***********************************************
*
* @Nom: FREMEN_sendFrame
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_sendFrame(int fd, char * frame) {
    write(fd, frame, 256);
}

/***********************************************
*
* @Nom: FREMEN_generateFrameLogout
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
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

/***********************************************
*
* @Nom: RsiControlC
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void RsiControlC(void) {

    //Printem el missatge de final d'execució.
    printF("\n\nAturant Fremen... \n\n");

    free(configuration.ip);
    free(configuration.directory);

    if (socket_fd > 0) {
        char * frame;

        frame = FRAME_CONFIG_generateFrame(1);
        frame = FREMEN_generateFrameLogout(frame, 'Q');
        FREMEN_sendFrame(socket_fd, frame);

        free(frame);

        printF("\nDesconnectat d’Atreides. Dew!\n\n");
    }

    if (socket_fd == -1) {
        socket_fd = 0;
    }

    free(user_name);
    close(socket_fd);

    //Acabem el programa enviant-nos a nosaltres mateixos el sigint.
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

/***********************************************
*
* @Nom: FREMEN_fillConfiguration
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
ConfigFremen FREMEN_fillConfiguration(char * argv) {
    char caracter = ' ', * cadena = NULL, * temp = NULL;
    int i = 0, fd, size = 0;
    ConfigFremen c;

    //Obertura del fitxer.
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

        temp = IOSCREEN_readUntilIntro(fd, caracter, i);
        size = strlen(temp);

        c.directory = (char * ) malloc(sizeof(char) * size);
        memset(c.directory, 0, size * sizeof(char));

        for (i = 1; temp[i] != '\0'; i++) {
            c.directory[i - 1] = temp[i];
        }

        close(fd);
        free(temp);
    }

    //Lectura del fitxer de config i tancament del file descriptor.
    return c;
}

/***********************************************
*
* @Nom: FREMEN_freeMemory
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_freeMemory(char * command, char ** command_array) {
    free(command);
    free(command_array);
}

/***********************************************
*
* @Nom: FREMEN_checkInputOnlyNumber
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
int FREMEN_checkInputOnlyNumber(char * input) {

    unsigned int i = 0;
    int count = 0;

    for (i = 0; i < strlen(input); i++) {
        if (input[i] >= '0' && input[i] <= '9') {
            count++;
        }
    }

    return count;

}

/***********************************************
*
* @Nom: FREMEN_checkNumberOfWords
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
int FREMEN_checkNumberOfWords(char * command, int words, char ** command_array) {

    if (strcasecmp(command, "login") == 0) {
        if (words == 3) {
            unsigned int checkOnlyNumber;

            checkOnlyNumber = FREMEN_checkInputOnlyNumber(command_array[2]);
            if (checkOnlyNumber == strlen(command_array[2])) {
                return 0;
            } else {
                printF("Comanda KO. Codi Postal incorrecte, només números.\n");
                return 1;
            }
            return 0;
        } else if (words < 3) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcasecmp(command, "search") == 0) {
        if (words == 2) {
            unsigned int checkOnlyNumber;

            checkOnlyNumber = FREMEN_checkInputOnlyNumber(command_array[1]);
            if (checkOnlyNumber == strlen(command_array[1])) {
                return 0;
            } else {
                printF("Comanda KO. Codi Postal incorrecte, només números.\n");
                return 1;
            }
        } else if (words < 2) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcasecmp(command, "send") == 0) {
        if (words == 2) {
            return 0;
        } else if (words < 2) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcasecmp(command, "photo") == 0) {
        if (words == 2) {
            return 0;
        } else if (words < 2) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    } else if (strcasecmp(command, "logout") == 0) {
        if (words == 1) {
            return 0;
        } else if (words < 1) {
            printF("Comanda KO. Falta paràmetres\n");
        } else {
            printF("Comanda KO. Massa paràmetres\n");
        }
        return 1;
    }

    //Si no és cap de les opcions es retorna un nombre 2 ja que segurament
    //sigui una comanda linux.
    return 2;
}

/***********************************************
*
* @Nom: FREMEN_configSocket
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
int FREMEN_configSocket(ConfigFremen config, char * command, char ** command_array) {

    struct sockaddr_in s_addr;
    int socket_fd;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printF("ERROR: crear socket del cliente\n");
        FREMEN_freeMemory(command, command_array);
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
    }

    return socket_fd;
}

/***********************************************
*
* @Nom: FREMEN_login
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_login(ConfigFremen configuration, char * command, char ** command_array) {
    socket_fd = FREMEN_configSocket(configuration, command, command_array);
    if (socket_fd < 1) {
        printF("ERROR: no se ha podido conectar el socket\n");
    }
}

/***********************************************
*
* @Nom: FREMEN_generateFrameLogin
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
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

/***********************************************
*
* @Nom: FREMEN_generateFrameSearch
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
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

/***********************************************
*
* @Nom: FREMEN_sendInfoPhoto
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
Photo FREMEN_sendInfoPhoto(char * frame, char type, char * file) {
    int i = 0, j = 0;
    char * md5 = NULL, * data_to_send, * out_file = NULL;
    struct stat stats;
    Photo p;

    strcpy(p.file_name, file);
    asprintf( & out_file, "%s/%s", configuration.directory, p.file_name);

    p.photo_fd = open(out_file, O_RDONLY);

    if (p.photo_fd < 0) {
        printF("La imatge no existeix...\n");
    } else {
        md5 = FRAME_CONFIG_getMD5(out_file);

        if (stat(out_file, & stats) == 0) {
            p.file_size = stats.st_size;
        }

        asprintf( & data_to_send, "%s*%d*%s", p.file_name, p.file_size, md5);

        frame[15] = type;

        for (i = 16; data_to_send[i - 16] != '\0'; i++) {
            frame[i] = data_to_send[i - 16];
        }

        for (j = i; j < 256; j++) {
            frame[j] = '\0';
        }

        FREMEN_sendFrame(socket_fd, frame);

        free(data_to_send);
        free(md5);
    }

    free(out_file);
    return p;
}

/***********************************************
*
* @Nom: FREMEN_receivePhotoInfo
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
Photo FREMEN_receivePhotoInfo(char data[240]) {
    int i, j, k;
    Photo p;
    char * number;

    i = 0;

    while (data[i] != '*') {
        p.file_name[i] = data[i];
        i++;
    }
    p.file_name[i] = '\0';
    i++;

    j = 0;
    number = (char * ) malloc(1 * sizeof(char));

    while (data[i] != '*') {
        number[j] = data[i];
        number = (char * ) realloc(number, j + 2);
        i++;
        j++;
    }
    number[j] = '\0';
    i++;

    p.file_size = atoi(number);

    k = 0;

    while (data[i] != '\0') {
        p.file_md5[k] = data[i];
        i++;
        k++;
    }
    p.file_md5[k] = '\0';

    free(number);

    return p;
}

/***********************************************
*
* @Nom: FREMEN_generateFrameSend
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_generateFrameSend(char * frame, char type, char data[240]) {
    int i = 0;

    frame[15] = type;

    for (i = 16; i < 256; i++) {
        frame[i] = data[i - 16];
    }
}

/***********************************************
*
* @Nom: FREMEN_sendPhoto
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_sendPhoto(Photo p) {
    int contador_trames = 0, num_trames = 0;
    char * frame, buffer[240];

    num_trames = p.file_size / 240;
    if (p.file_size % 240 != 0) {
        num_trames++;
    }

    while (contador_trames < num_trames) {
        memset(buffer, 0, sizeof(buffer));
        read(p.photo_fd, buffer, 240);

        frame = FRAME_CONFIG_generateFrame(1);
        FREMEN_generateFrameSend(frame, 'D', buffer);

        FREMEN_sendFrame(socket_fd, frame);

        contador_trames++;

        free(frame);
        usleep(300);
    }

    close(p.photo_fd);
}

/***********************************************
*
* @Nom: FREMEN_generateFramePhoto
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_generateFramePhoto(char * frame, char type, char data[240]) {
    int i = 0, j = 0;

    frame[15] = type;

    for (i = 16; data[i - 16] != '\0'; i++) {
        frame[i] = data[i - 16];
    }

    for (j = i; j < 256; j++) {
        frame[j] = '\0';
    }
}

/***********************************************
*
* @Nom: FREMEN_receivePhoto
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_receivePhoto(Photo p) {
    Frame frame;
    int out, contador_trames = 0;
    char * md5 = NULL, * out_file = NULL, cadena[200], * trama = NULL;

    sprintf(cadena, "Guardada com %s\n", p.file_name);
    write(STDOUT_FILENO, cadena, sizeof(char) * strlen(cadena));

    asprintf( & out_file, "%s/%s", configuration.directory, p.file_name);

    out = open(out_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    int num_trames = p.file_size / 240;
    if (p.file_size % 240 != 0) {
        num_trames++;
    }

    while (contador_trames < num_trames) {

        frame = FRAME_CONFIG_receiveFrame(socket_fd);

        if (contador_trames == num_trames - 1 && p.file_size % 240 != 0) {
            write(out, frame.data, p.file_size % 240);
        } else {
            write(out, frame.data, 240);
        }

        contador_trames++;
    }
    close(out);

    md5 = FRAME_CONFIG_getMD5(out_file);

    if (md5 != NULL) {
        if (strcmp(p.file_md5, md5) != 0) {
            printF("Error: Les fotos no són iguals! \n");
            trama = FRAME_CONFIG_generateCustomFrame(1, 'R', 1);
        } else {
            trama = FRAME_CONFIG_generateCustomFrame(1, 'I', 0);
        }
        FREMEN_sendFrame(socket_fd, trama);
        free(trama);
        free(md5);
    }

    free(out_file);
}

/***********************************************
*
* @Nom: FREMEN_countAsterisk
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
int FREMEN_countAsterisk(int num_asterics, char data[240]) {
    //Comptem quants asterics hi han, per veure si hem de rebre més trames de search
    for (int l = 0; data[l] != '\0'; l++) {
        if (data[l] == '*') num_asterics++;
    }

    return num_asterics;
}


/***********************************************
*
* @Nom: FREMEN_showSearchReceived
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
void FREMEN_showSearchReceived(char data[240], char * postal_code) {

    int i, k, num_searched_users = 0, num_asterics = 0;
    char * num_searched_users_str = NULL, * name = NULL, * id_user = NULL, cadena[100], * search_data;
    Frame frame_received;

    i = 0;

    asprintf(& search_data, "%s", data);

    if (strlen(search_data) > 1) {
        num_searched_users_str = (char * ) malloc(1 * sizeof(char));
        while (search_data[i] != '*') {
            num_searched_users_str[i] = search_data[i];
            num_searched_users_str = (char * ) realloc(num_searched_users_str, i + 2);
            i++;
        }
        num_searched_users_str[i] = '\0';
        num_searched_users = atoi(num_searched_users_str);

        num_asterics = FREMEN_countAsterisk(num_asterics, search_data);

        while (num_asterics/2 != num_searched_users) {
            printf("\nNum users %d num ast %d\n", num_searched_users, num_asterics);
            frame_received = FRAME_CONFIG_receiveFrame(socket_fd);

            asprintf(& search_data, "%s%s", search_data, frame_received.data);
            num_asterics = FREMEN_countAsterisk(num_asterics, search_data);

            if (num_asterics > num_searched_users*2) break;
        }

        i++;

        sprintf(cadena, "\nHi ha %s persones humanes a %s\n", num_searched_users_str, postal_code);
        write(STDOUT_FILENO, cadena, strlen(cadena));

        for (int j = 0; j < num_searched_users; j++) {
            k = 0;
            name = (char * ) malloc(1 * sizeof(char));

            while (search_data[i] != '*') {
                name[k] = search_data[i];
                name = (char * ) realloc(name, k + 2);
                i++;
                k++;
            }

            name[k] = '\0';
            i++;

            id_user = (char * ) malloc(1 * sizeof(char));
            k = 0;

            while (search_data[i] != '*' && search_data[i] != '\0') {
                id_user[k] = search_data[i];
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
    } else {
        sprintf(cadena, "\nHi ha zero persones humanes a %s\n", postal_code);
        write(STDOUT_FILENO, cadena, strlen(cadena));
    }
    free(search_data);
}

/***********************************************
*
* @Nom: FREMEN_promptChoice
* @Finalitat:
* @Parametres:
* @Retorn:
*
************************************************/
int FREMEN_promptChoice(ConfigFremen configuration) {
    char * command = NULL, * frame = NULL, cadena[200];
    char * ( * command_array);
    int i = 0, num_of_words = 0, isok = 0;
    Photo p;

    //Lectura per pantalla de la comanda y tractament per quedar-nos amb una cadena.
    printF("$ ");
    command = IOSCREEN_readUntilIntro(STDIN_FILENO, '\n', 0);

    //Revisió si la cadena només conté intros.
    if (command[0] == '\0') {
        free(command);
        return 0;
    }

    //Revisió si la cadena només conté espais.
    if (IOSCREEN_isEmpty(command) == 1) {
        free(command);
        return 0;
    }

    //Tractament del comand per separar la comanda de sortida.
    command_array = (char ** ) malloc(sizeof(char * ));
    command_array[i] = strtok(command, " ");

    while (command_array[i] != NULL) {
        num_of_words++;
        command_array = realloc(command_array, (num_of_words + 1) * sizeof(char * ));
        command_array[++i] = strtok(NULL, " ");
    }

    //Check del nombre de parametres.
    isok = FREMEN_checkNumberOfWords(command_array[0], num_of_words, command_array);

    //Comanda custom ok
    if (isok == 0) {
        if (strcasecmp(command_array[0], "logout") == 0) {
            if (socket_fd > 0) {

                frame = FRAME_CONFIG_generateFrame(1);
                frame = FREMEN_generateFrameLogout(frame, 'Q');
                FREMEN_sendFrame(socket_fd, frame);

                free(frame);

                close(socket_fd);
                socket_fd = 0;

                printF("\nDesconnectat d’Atreides. Dew!\n\n");

                FREMEN_freeMemory(command, command_array);
                raise(SIGINT);

            } else {
                printF("No puc fer logout si no estic connectat al servidor...\n");
            }

        } else if (strcasecmp(command_array[0], "login") == 0) {
            if (socket_fd == 0) {

                frame = FRAME_CONFIG_generateFrame(1);
                FREMEN_login(configuration, command, command_array);

                if (socket_fd > 0) {

                    frame = FREMEN_generateFrameLogin(frame, 'C', command_array[1], command_array[2]);
                    FREMEN_sendFrame(socket_fd, frame);

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

            } else if (socket_fd > 0) {
                printF("No puc fer login si ja estic connectat al servidor amb un altre usuari...\n");
            }

        } else if (strcasecmp(command_array[0], "search") == 0) {
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

        } else if (strcasecmp(command_array[0], "send") == 0) {
            if (socket_fd > 0) {
                frame = NULL;

                frame = FRAME_CONFIG_generateFrame(1);
                p = FREMEN_sendInfoPhoto(frame, 'F', command_array[1]);

                if (p.photo_fd > 0) {
                    FREMEN_sendPhoto(p);

                    Frame frame_received;
                    frame_received = FRAME_CONFIG_receiveFrame(socket_fd);

                    if (frame_received.type == 'I') {
                        printF("Foto enviada amb èxit a Atreides. \n");
                    } else {
                        printF("Error a l'enviar la imatge a Atreides. \n");
                    }
                }

                free(frame);
            } else {
                printF("No es pot enviar la imatge perque no esteu loginats al servidor... \n");
            }

        } else if (strcasecmp(command_array[0], "photo") == 0) {
            if (socket_fd > 0) {
                frame = NULL;

                frame = FRAME_CONFIG_generateFrame(1);
                FREMEN_generateFramePhoto(frame, 'P', command_array[1]);
                FREMEN_sendFrame(socket_fd, frame);

                Frame frame_received_data;
                frame_received_data = FRAME_CONFIG_receiveFrame(socket_fd);

                if (strcmp(frame_received_data.data, "FILE NOT FOUND") == 0) {
                    printF("No hi ha foto registrada\n");
                } else {
                    Photo p = FREMEN_receivePhotoInfo(frame_received_data.data);

                    FREMEN_receivePhoto(p);
                }
                free(frame);
            } else {
                printF("No es pot enviar la imatge perque no esteu loginats al servidor... \n");
            }

        }

        FREMEN_freeMemory(command, command_array);

        return 1;
        //Comanda custom KO, per parametros
    } else if (isok == 1) {
        FREMEN_freeMemory(command, command_array);
        free(frame);
        return 0;
    } else {
        pid_t pid;

        //Creació del fork
        if ((pid = fork()) < 0) {
            FREMEN_freeMemory(command, command_array);
            printF("Error al crear el Fork\n");
            exit(1);
        }
        //Entrada al codi del fill.
        else if (pid == 0) {
            //Execució de la comanda i tractament si ha anat malament.
            if (execvp(command_array[0], command_array) < 0) {

                free(user_name);

                FREMEN_freeMemory(command, command_array);
                free(frame);
                free(configuration.ip);
                free(configuration.directory);
                close(socket_fd);

                printF("Error al executar la comanda!\n");
                exit(EXIT_FAILURE);
            }
            //Codi del pare
        } else {
            //Espera a la execució de la comanda
            wait(NULL);

            //alliberament de memoria
            FREMEN_freeMemory(command, command_array);
        }
        return 0;
    }
}

/***********************************************
*
* @Nom: Main
* @Finalitat: control flow of the program.
* @Parametres:
* @Retorn:
*
************************************************/
int main(int argc, char ** argv) {

    int command = 0;

    //Check dels arguments d'entrada
    if (argc != 2) {
        printF("Error, falta el nom de fitxer de configuracio.\n");
        return -1;
    }

    //Assignació del signal de CtrlC a la nostra funció
    signal(SIGINT, (void * ) RsiControlC);

    //Lectura del fitxer de configuració i tancament del seu file descriptor.
    configuration = FREMEN_fillConfiguration(argv[1]);

    //Inicializació de fremen.
    printF("Benvingut a Fremen\n");
    while (command == 0 || command == 1) {
        command = FREMEN_promptChoice(configuration);
    }

    raise(SIGINT);

    return 0;
}