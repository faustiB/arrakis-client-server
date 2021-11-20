#include "atreides.h"

void RsiControlC(void);
Config configuration;
User *users;

/* ********************************************************************
*
* @Nombre : RsiControlC
* @Def : Rutina asociada a la interrupción Control+C.
*
********************************************************************* */
void RsiControlC(void){
    //Printamos el mensaje de adiós al sistema
	printF("\n\nDesconnectat d’Atreides. Dew!\n\n");

    free(configuration.ip);
    free(configuration.directory);

    //Terminamos el programa enviándonos a nosotros mismos el signal de SIGINT
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}


/* ********************************************************************
*
* @Nombre : ATREIDES_readUntilIntro
* @Def : Función para leer hasta un intro.
*
********************************************************************* */
char *ATREIDES_readUntilIntro(int fd, char caracter, int i) {
    char *buffer = (char *) malloc(1 * sizeof(char));

    while (caracter != '\n' || i == 0) {

        read(fd, &caracter, sizeof(char));
        buffer = (char *) realloc(buffer, i + 1);
        buffer[i] = caracter;

        if (caracter == '\n' || caracter == '\0') {
            buffer[i] = '\0';
            return buffer;
        }

        i++;
    }

    return buffer;
}

char * ATREIDES_readLine(int fd, char delimiter){

	 char * msg = malloc(1);
	 char current;
	 int i = 0;
	 int len = 0;
	 while ((len += read(fd, &current, 1)) > 0){

		 msg[i] = current;
		 msg = (char *) realloc(msg, ++i + 1);
		 if (current == delimiter)
             break;
	 }
	 msg[i-1] = '\0';

	 return msg;
 }

/* ********************************************************************
*
* @Nombre : ATREIDES_fillConfiguration
* @Def : Función para leer el fichero de configuración, y devolverlo en nuestro struct.
*
********************************************************************* */
Config ATREIDES_fillConfiguration(char * argv) {
    char caracter = ' ', *cadena = NULL;
    int i = 0, fd;
    Config c;

    //Apertura del fichero
    fd = open(argv, O_RDONLY);

    if (fd < 0 ) {
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
    char c;
    int i = 0, fd, numUsers, n;

    User *u;

    //Apertura del fichero
    fd = open("Atreides/users_memory.txt", O_RDONLY);

    if (fd < 0 ) {
      printF("Fitxer de usuaris erroni\n");
      raise(SIGINT);
    } else {
        //Leemos el número de usuarios
        do{
            n = read(fd, &c, sizeof(char));
            numUsers[i++] = c;
        } while ( n!=0 && c!='\n');

        //BORRAR
        printf("Tenemos %d usuarios guardados...\n", numUsers);

        u = (User *) malloc(sizeof(User));
        //free(cadena);

        close(fd);
    }

    return u;
}

/* ********************************************************************
*
* @Nombre : Main
* @Def : Función principal.
*
********************************************************************* */
int main(int argc, char **argv) {

    //Check de argumentos de entrada
    if (argc != 2) {
		printF("Error, falta el nom de fitxer de configuracio.\n");
        return -1;
    }

    //Assignación del signal de CtrlC a nuestra función
    signal(SIGINT, (void*)RsiControlC);

    //Lectura del fichero de configuración y cierre de su file descriptor.
    configuration = ATREIDES_fillConfiguration(argv[1]);

    //Carga de usuarios en el struct
    //users = ATREIDES_fillUsers();

    raise(SIGINT);

    return 0;
}