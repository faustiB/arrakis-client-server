#include "atreides.h"

void RsiControlC(void);

Config configuration;
int num_users, socket_fd;
User * users;

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

    for (int i = 0; i < num_users; i++) {
        free(users[i].username);
        free(users[i].postal_code);
    }
    free(users);

    close(socket_fd);

    //Terminamos el programa enviándonos a nosotros mismos el signal de SIGINT
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_read_until
 * @Def : Función para leer una linea
 *
 ********************************************************************* */
char * ATREIDES_read_until(int fd, char end) {
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
 * @Nombre : ATREIDES_readUntilIntro
 * @Def : Función para leer hasta un intro.
 *
 ********************************************************************* */
char * ATREIDES_readUntilIntro(int fd, char caracter, int i) {
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
 * @Nombre : ATREIDES_generateFrame
 * @Def : ceación de una trama de Atreides
 *
 ********************************************************************* */
char * ATREIDES_generateFrame() {
    char * frame;
    int i = 0;

    frame = (char * ) malloc(sizeof(char) * 256);

    sprintf((char * ) frame, "ATREIDES");

    for (i = strlen((char * ) frame); i < 15; i++) {
        frame[i] = '\0';
    }

    return frame;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_generateFrameLogin
 * @Def : ceación y generación de la trama de respuesta para login.
 *
 ********************************************************************* */
char * ATREIDES_generateFrameLogin(char * frame, char type, int id) {

    int i = 0, j = 0;

    char * buffer, id_str[3];

    frame[15] = type;

    snprintf(id_str, 3, "%d", id);
    asprintf( & buffer, "%s", id_str);

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
 * @Nombre : ATREIDES_generateFrameSearch
 * @Def : ceación y generación de la trama de respuesta para search.
 *
 ********************************************************************* */
char * ATREIDES_generateFrameSearch(char * frame, char type, char * str) {

    int i = 0, j = 0;

    frame[15] = type;

    for (i = 16; str[i - 16] != '\0'; i++) {
        frame[i] = str[i - 16];
    }

    for (j = i; j < 256; j++) {
        frame[j] = '\0';
    }

    //free(str);
    return frame;

}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_fillUsers
 * @Def : Función para leer el fichero de usuarios, y devolverlo en nuestro struct.
 *
 ********************************************************************* */
User * ATREIDES_fillUsers() {
    char caracter = ' ', * buffer;
    int fd, i = 0;
    User * users_read;

    //Apertura del fichero
    fd = open("Atreides/users_memory.txt", O_RDONLY);

    if (fd < 0) {
        printF("Fitxer de usuaris erroni\n");
        raise(SIGINT);
    } else {

        buffer = ATREIDES_readUntilIntro(fd, caracter, i);
        num_users = atoi(buffer);
        users_read = (User * ) malloc(sizeof(User) * num_users);
        free(buffer);

        i = 0;
        while (i < num_users) {

            buffer = NULL;
            buffer = ATREIDES_read_until(fd, '-');
            users_read[i].id = atoi(buffer);

            users_read[i].username = ATREIDES_read_until(fd, '-');

            users_read[i].postal_code = ATREIDES_read_until(fd, '\n');

            free(buffer);
            i++;
        }

        close(fd);
    }

    return users_read;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_addUser
 * @Def : ...
 *
 ********************************************************************* */
void ATREIDES_addUser(User u) {
    char * buffer, num_users_str[3], id_str[3], id_str_new[3];
    int fd;

    fd = open("Atreides/users_memory.txt", O_CREAT | O_RDWR, 0666);

    if (fd < 0) {
        printF("Fitxer de usuaris erroni\n");
        raise(SIGINT);
    } else {
        snprintf(num_users_str, 3, "%d", num_users);
        asprintf( & buffer, "%s\n", num_users_str);
        write(fd, buffer, sizeof(char) * strlen(buffer));

        for (int i = 0; i < num_users - 1; i++) {

            snprintf(id_str, 3, "%d", users[i].id);
            asprintf( & buffer, "%s-%s-%s\n", id_str, users[i].username, users[i].postal_code);
            write(fd, buffer, sizeof(char) * strlen(buffer));

        }

        snprintf(id_str_new, 3, "%d", u.id);
        asprintf( & buffer, "%s-%s-%s\n", id_str_new, u.username, u.postal_code);
        write(fd, buffer, sizeof(char) * strlen(buffer));

        memset( & users, 0, sizeof(users));

        close(fd);

        users = ATREIDES_fillUsers();

    }
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_receiveUser
 * @Def : Recepcion de usuario
 *
 ********************************************************************* */
User ATREIDES_receiveUser(char data[240]) {

    int i, j;
    User u;

    i = 0;
    u.username = (char * ) malloc(1 * sizeof(char));
    while (data[i] != '*') {
        u.username[i] = data[i];
        u.username = (char * ) realloc(u.username, i + 2);
        i++;
    }
    u.username[i] = '\0';

    i++;

    j = 0;
    u.postal_code = (char * ) malloc(1 * sizeof(char));

    while (data[i] != '\0') {
        u.postal_code[j] = data[i];
        u.postal_code = (char * ) realloc(u.postal_code, i + 2);
        i++;
        j++;
    }
    u.postal_code[j] = '\0';

    //set del id a 0 , en el caso que tenga, si no tiene se le asignará uno nuevo .
    u.id = 0;

    return u;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_receiveUser
 * @Def : Recepcion de usuario
 *
 ********************************************************************* */
User ATREIDES_receiveSearch(char data[240]) {
    int i, j, k;
    User u;
    char * id;

    i = 0;

    u.username = (char * ) malloc(1 * sizeof(char));
    while (data[i] != '*') {
        u.username[i] = data[i];
        u.username = (char * ) realloc(u.username, i + 2);
        i++;
    }
    u.username[i] = '\0';
    i++;

    j = 0;
    id = (char * ) malloc(1 * sizeof(char));

    while (data[i] != '*') {
        id[j] = data[i];
        id = (char * ) realloc(id, j + 2);
        i++;
        j++;
    }
    id[j] = '\0';
    i++;

    u.id = atoi(id);

    k = 0;
    u.postal_code = (char * ) malloc(1 * sizeof(char));

    while (data[i] != '\0') {
        u.postal_code[k] = data[i];
        u.postal_code = (char * ) realloc(u.postal_code, k + 2);
        i++;
        k++;
    }
    u.postal_code[k] = '\0';

    free(id);
    return u;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_sendFrame
 * @Def : Envío de la trama
 *
 ********************************************************************* */
void ATREIDES_sendFrame(int fd, char * frame) {
    write(fd, frame, 256);
    printF("Enviada resposta\n");
}
/* ********************************************************************
 *
 * @Nombre : ATREIDES_receiveFrame
 * @Def : Rececpción de trama
 *
 ********************************************************************* */
Frame ATREIDES_receiveFrame(int fd) {
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
 * @Nombre : ATREIDES_searchUsers
 * @Def : Buscar Ususarios por  codigo postal
 *
 ********************************************************************* */
char * ATREIDES_searchUsers(User u) {

    //Li passem numusers i a mesura que afegim si strlen(res) i ja hem omplert la trama,
    // creem un altre string per passar.

    int num_users_pc = 0, i;
    char * res, num_users_pc_str[3], id_str[3];

    for (i = 0; i < num_users; i++) {
        if (strcmp(users[i].postal_code, u.postal_code) == 0) {
            num_users_pc++;

        }
    }

    snprintf(num_users_pc_str, 3, "%d", num_users_pc);
    asprintf( & res, "%s", num_users_pc_str);

    for (i = 0; i < num_users; i++) {
        if (strcmp(users[i].postal_code, u.postal_code) == 0) {

            snprintf(id_str, 3, "%d", users[i].id);
            asprintf( & res, "%s*%s*%s", res, users[i].username, id_str);

        }
    }

    return res;

}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_threadClient
 * @Def : función de thread, por cada cliente.
 *
 ********************************************************************* */
void * ATREIDES_threadClient(void * fdClient) {

    int fd = * ((int * ) fdClient);
    Frame frame;
    int i, exit;
    User u;
    char * frame_send = NULL, cadena[100], * search_data;

    exit = 0;
    while (!exit) {

        frame = ATREIDES_receiveFrame(fd);

        //mirar tipo y casuística.
        switch (frame.type) {
        case 'C':
            //Login
            u = ATREIDES_receiveUser(frame.data);

            i = 0;
            for (i = 0; i < num_users; i++) {
                if (strcmp(u.username, users[i].username) == 0) {
                    u.id = users[i].id;
                }
            }

            if (u.id == 0) {
                num_users++;
                u.id = num_users;
                ATREIDES_addUser(u);
            }

            sprintf(cadena, "\nRebut Login de %s %s\nAssignat a ID %d.\n", u.username, u.postal_code, u.id);
            write(STDOUT_FILENO, cadena, sizeof(char) * strlen(cadena));

            frame_send = ATREIDES_generateFrame();
            frame_send = ATREIDES_generateFrameLogin(frame_send, 'O', u.id);

            ATREIDES_sendFrame(fd, frame_send);

            free(u.username);
            free(u.postal_code);
            free(frame_send);
            break;

        case 'S':
            //search
            u = ATREIDES_receiveSearch(frame.data);

            sprintf(cadena, "\nRebut Search %s de %s %d\n", u.postal_code, u.username, u.id);
            write(STDOUT_FILENO, cadena, strlen(cadena));

            search_data = ATREIDES_searchUsers(u);

            frame_send = ATREIDES_generateFrame();
            frame_send = ATREIDES_generateFrameSearch(frame_send, 'S', search_data);

            ATREIDES_sendFrame(fd, frame_send);

            free(search_data);
            free(frame_send);
            free(u.username);
            free(u.postal_code);
            break;

        case 'Q':
            exit = 1;

            u = ATREIDES_receiveUser(frame.data);
            sprintf(cadena, "\nRebut Logout de  %s %s \nDesconnectat d'Atreides.\n", u.username, u.postal_code);
            write(STDOUT_FILENO, cadena, strlen(cadena));

            break;
        }

    }

    close(fd);

    //guardarem el pid del thread per a dins de la rsi, poder tancar-lo. Afegir valor al user.
    pthread_detach(pthread_self());
    pthread_cancel(pthread_self());

    return NULL;

}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_readDelimiter
 * @Def : Función para leer una linea
 *
 ********************************************************************* */
char * ATREIDES_readDelimiter(int fd, char delimiter) {

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
 * @Nombre : ATREIDES_fillConfiguration
 * @Def : Función para leer el fichero de configuración, y devolverlo en nuestro struct.
 *
 ********************************************************************* */
Config ATREIDES_fillConfiguration(char * argv) {
    char caracter = ' ', * cadena = NULL;
    int i = 0, fd;
    Config c;

    //Apertura del fichero
    fd = open(argv, O_RDONLY);

    if (fd < 0) {
        printF("Fitxer de configuració erroni\n");
        raise(SIGINT);

    } else {
        c.ip = ATREIDES_readUntilIntro(fd, caracter, i);
        cadena = ATREIDES_readUntilIntro(fd, caracter, i);
        c.port = atoi(cadena);
        free(cadena);

        c.directory = ATREIDES_readUntilIntro(fd, caracter, i);
        close(fd);
        printF("Llegit el fitxer de configuració\n");
    }

    return c;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_configSocket
 * @Def : Extrae los datos del fichero config y prepara el socket
 * @Arg : Out: el fd de la nueva conexion
 *
 ********************************************************************* */

int ATREIDES_configSocket(Config config) {

    struct sockaddr_in s_addr; //cliente_addr;
    int fdSocket = -1;

    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket < 0) {
        printF("ERROR durant la creacio del socket\n");
        return -1;
    }

    memset( & s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(config.port);
    s_addr.sin_addr.s_addr = inet_addr(config.ip);

    if (bind(fdSocket, (void * ) & s_addr, sizeof(s_addr)) < 0) {
        printF("ERROR fent el bind\n");
        return -1;
    }

    listen(fdSocket, 3);

    return fdSocket;
}

/* ********************************************************************
 *
 * @Nombre : checkTrama
 * @Def : Comprueba si existe el nombre
 * @Arg : In : nombre a buscar
 *        Out: un boleano que declara si se
 *             ha encontrado una coincidencia
 *
 ********************************************************************* */

int checkTrama() {

    return -1;
}

/* ********************************************************************
 *
 * @Nombre : Main
 * @Def : Función principal.
 *
 ********************************************************************* */
int main(int argc, char ** argv) {
    int clientFD;

    //Check de argumentos de entrada
    if (argc != 2) {
        printF("Error, falta el nom de fitxer de configuracio.\n");
        return -1;
    }

    //Assignación del signal de CtrlC a nuestra función
    signal(SIGINT, (void * ) RsiControlC);

    //Lectura del fichero de configuración y cierre de su file descriptor.
    configuration = ATREIDES_fillConfiguration(argv[1]);

    //Carga de usuarios en el struct
    users = ATREIDES_fillUsers();

    //Config del socket
    socket_fd = ATREIDES_configSocket(configuration);

    if (socket_fd < 1) {
        printF("ERROR: imposible preparar el socket\n");
        close(socket_fd);
        raise(SIGINT);
    }

    while (1) {
        printF("Esperant connexions...\n");
        clientFD = accept(socket_fd, (struct sockaddr * ) NULL, NULL);

        pthread_t thrd;
        pthread_create( & thrd, NULL, ATREIDES_threadClient, & clientFD);
    }

    raise(SIGINT);

    return 0;
}