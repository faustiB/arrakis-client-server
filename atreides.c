
#include "atreides.h"
#include "ioscreen.h"

void RsiControlC(void);

ConfigAtreides configuration;
int num_users, socket_fd;
User * users;

/* ********************************************************************
 *
 * @Nombre : UpdateFile
 * @Def :
 *
 ********************************************************************* */
void UpdateFile() {
    char cadena[100];
    int fd;

    fd = open("Atreides/users_memory.txt", O_CREAT | O_RDWR, 0666);

    if (fd < 0) {
        printF("Fitxer de usuaris erroni\n");
        raise(SIGINT);
    } else {

        sprintf(cadena, "%d\n", num_users);
        write(fd, cadena, sizeof(char) * strlen(cadena));

        for (int i = 0; i < num_users; i++) {
            sprintf(cadena,  "%d-%s-%s\n", users[i].id, users[i].username, users[i].postal_code);
            write(fd, cadena, sizeof(char) * strlen(cadena));
            memset(cadena,0,strlen(cadena));
        }

        close(fd);
    }
}

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

    UpdateFile();

    for (int i = 0; i < num_users; i++) {
        free(users[i].username);
        free(users[i].postal_code);
        if (users[i].file_descriptor != -1) {
            close(users[i].file_descriptor);
            pthread_detach(users[i].thread);
            pthread_cancel(users[i].thread);
        }
    }
    free(users);

    close(socket_fd);

    //Terminamos el programa enviándonos a nosotros mismos el signal de SIGINT
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
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
        //printF("Fitxer de usuaris erroni\n");

        fd = open("Atreides/users_memory.txt", O_CREAT | O_RDWR , 0666);
        if(fd < 0){

            printF("Error creant fitxer\n");
            raise(SIGINT);

        } else {

            write(fd,"1\n",2);
            write(fd,"1-Admin-00000\n",strlen("1-Admin-00000\n"));

        }

    }

    buffer = IOSCREEN_readUntilIntro(fd, caracter, i);
    num_users = atoi(buffer);
    users_read = (User * ) malloc(sizeof(User) * num_users);
    free(buffer);

    i = 0;
    while (i < num_users) {

        buffer = NULL;
        buffer = IOSCREEN_read_until(fd, '-');
        users_read[i].id = atoi(buffer);

        users_read[i].username = IOSCREEN_read_until(fd, '-');

        users_read[i].postal_code = IOSCREEN_read_until(fd, '\n');
        users_read[i].file_descriptor = -1;

        free(buffer);
        i++;
    }

    close(fd);


    return users_read;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_addUser
 * @Def : ...
 *
 ********************************************************************* */
void ATREIDES_addUser(User u) {

    users = (User *) realloc (users, ((num_users) * sizeof(User)));
    users[num_users-1].username = (char * ) malloc(sizeof(u.username));
    users[num_users-1].postal_code = (char * ) malloc(sizeof(u.postal_code));

    users[num_users-1].id = u.id;
    strcpy(users[num_users-1].username, u.username);
    strcpy(users[num_users-1].postal_code, u.postal_code);
    users[num_users-1].file_descriptor = u.file_descriptor;
    users[num_users-1].thread = u.thread;
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
        u.postal_code = (char * ) realloc(u.postal_code, j + 2);
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
 * @Nombre : ATREIDES_receiveSearch
 * @Def : Recepcion de trama search
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

Photo ATREIDES_receiveSendInfo (char data[240]) {
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

    printf("\nPhoto rebuda %s %d %s\n", p.file_name, p.file_size, p.file_md5);
    return p;
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
 * @Nombre : ATREIDES_searchUsers
 * @Def : Buscar Ususarios por  codigo postal
 *
 ********************************************************************* */
char * ATREIDES_searchUsers(User u) {

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
            printf("\n Res %s %ld", res, strlen(res));
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
    Photo photo;
    int i, exit;
    User u;
    char * frame_send, cadena[100], * search_data;

    exit = 0;
    while (!exit) {

        frame = FRAME_CONFIG_receiveFrame(fd);

        //mirar tipo y casuística.
        switch (frame.type) {
        case 'C':
            //Login
            u = ATREIDES_receiveUser(frame.data);

            //assignem el file descriptor obert per tancarlo al final
            u.file_descriptor = fd;
            u.thread = pthread_self();

            i = 0;
            for (i = 0; i < num_users; i++) {
                if ( ( strcmp(u.username, users[i].username) == 0 ) && ( strcmp(u.postal_code, users[i].postal_code) == 0 ) ) {
                    u.id = users[i].id;
                    users[i].file_descriptor = fd;
                    users[i].thread = pthread_self();
                }
            }

            if (u.id == 0) {
                //Semaforo para sumar uno a la vez
                num_users++;
                u.id = num_users;
                ATREIDES_addUser(u);
            }

            sprintf(cadena, "\nRebut Login de %s %s\nAssignat a ID %d.\n", u.username, u.postal_code, u.id);
            write(STDOUT_FILENO, cadena, sizeof(char) * strlen(cadena));

            free(u.username);
            free(u.postal_code);

            frame_send = FRAME_CONFIG_generateFrame(2);
            frame_send = ATREIDES_generateFrameLogin(frame_send, 'O', u.id);

            ATREIDES_sendFrame(fd, frame_send);

            free(frame_send);
            break;

        case 'S':
            //search
            u = ATREIDES_receiveSearch(frame.data);

            sprintf(cadena, "\nRebut Search %s de %s %d\n", u.postal_code, u.username, u.id);
            write(STDOUT_FILENO, cadena, strlen(cadena));

            search_data = ATREIDES_searchUsers(u);

            frame_send = FRAME_CONFIG_generateFrame(2);
            frame_send = ATREIDES_generateFrameSearch(frame_send, 'S', search_data);

            ATREIDES_sendFrame(fd, frame_send);

            free(search_data);
            free(frame_send);

            free(u.username);
            free(u.postal_code);
            break;

            case 'F':
            //send

            photo = ATREIDES_receiveSendInfo(frame.data);
            printf("\nTamany %d\n", photo.file_size);
            break;

        case 'Q':
            exit = 1;

            u = ATREIDES_receiveUser(frame.data);
            
            sprintf(cadena, "\nRebut Logout de  %s %s \nDesconnectat d'Atreides.\n", u.username, u.postal_code);
            write(STDOUT_FILENO, cadena, strlen(cadena));

            //guardarem el pid del thread per a dins de la rsi, poder tancar-lo. Afegir valor al user.
            pthread_detach(pthread_self());
            pthread_cancel(pthread_self());

            free(u.username);
            free(u.postal_code);

            break;
        }

    }

    close(fd);

    return NULL;
}

/* ********************************************************************
 *
 * @Nombre : ATREIDES_fillConfiguration
 * @Def : Función para leer el fichero de configuración, y devolverlo en nuestro struct.
 *
 ********************************************************************* */
ConfigAtreides ATREIDES_fillConfiguration(char * argv) {
    char caracter = ' ', * cadena = NULL;
    int i = 0, fd;
    ConfigAtreides c;

    //Apertura del fichero
    fd = open(argv, O_RDONLY);

    if (fd < 0) {
        printF("Fitxer de configuració erroni\n");
        raise(SIGINT);

    } else {
        c.ip = IOSCREEN_readUntilIntro(fd, caracter, i);
        cadena = IOSCREEN_readUntilIntro(fd, caracter, i);
        c.port = atoi(cadena);
        free(cadena);

        c.directory = IOSCREEN_readUntilIntro(fd, caracter, i);
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

int ATREIDES_configSocket(ConfigAtreides config) {

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