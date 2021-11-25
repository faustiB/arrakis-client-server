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
 * @Nombre : ATREIDES_threadClient
 * @Def : función de thread, por cada cliente.
 *
 ********************************************************************* */
void * ATREIDES_threadClient(void * fdClient){

	 int fd = *((int*)fdClient);
   Frame frame;
   int i, exit;

     char * frame_read = NULL;

     frame_read = (char * ) malloc(256 * sizeof(char));

     exit = 0;
     while (!exit){

         read(fd, frame_read, sizeof(char) * 256);

         i = 0;
         while (i<15){
           frame.origin[i] = frame_read[i];
           i++;
         }

        frame.type = frame_read[15];

        i = 16;
        while (i<256){
          frame.data[i-16] = frame_read[i];
          i++;
        }

         printf("%s - %c - %s \n", frame.origin,frame.type,frame.data );

         //mirar tipo y casuística.

         switch (frame.type) {
              case 'C':
                //Login
                break;

              case 'S':
                  //search
                  break;

              case 'Q':
                exit = 1;
                 //logout
                break;
         }


     }



     close(fd);
     free(frame_read);
	   pthread_cancel(pthread_self());
	   pthread_detach(pthread_self());


	   return NULL;

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
 * @Nombre : ATREIDES_fillUsers
 * @Def : Función para leer el fichero de usuarios, y devolverlo en nuestro struct.
 *
 ********************************************************************* */
User * ATREIDES_fillUsers() {
    char caracter = ' ', * buffer;
    int fd, i = 0;
    User * users_read;

    //
    // Leer de fichero de config
    //

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

        printF("Llegit fitxer de usuaris\n");
        close(fd);
    }

    return users_read;
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
        printF("ERROR: creacion de socket\n");
        return -1;
    }

    memset( & s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(config.port);
    s_addr.sin_addr.s_addr = inet_addr(config.ip);

    if (bind(fdSocket, (void * ) & s_addr, sizeof(s_addr)) < 0) {
        printF("ERROR: bind\n");
        return -1;
    }

    listen(fdSocket, 3);
    //socklen_t cliente_len = sizeof(cliente_addr);

    printF("Esperando nueva conexion... \n");
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


    while(1) {
        printF("Esperant connexions...\n");
        clientFD = accept(socket_fd, (struct sockaddr*) NULL, NULL);

        printF("\nNou client connectat!\nMissatge: ");
        pthread_t thrd;
        pthread_create(&thrd, NULL, ATREIDES_threadClient, &clientFD);
    }

    raise(SIGINT);

    return 0;
}
