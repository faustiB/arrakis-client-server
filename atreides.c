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

/* ********************************************************************
*
* @Nombre : ATREIDES_readLine
* @Def : Función para leer una linea
*
********************************************************************* */
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
 * @Nombre : ATREIDES_read_until
 * @Def : Función para leer una linea
 *
 ********************************************************************* */
 char* ATREIDES_read_until(int fd, char end) {
     int i = 0, size;
     char c = '\0';
     char* string = (char*)malloc(sizeof(char));

     while (1) {
         size = read(fd, &c, sizeof(char));

         if (c != end && size > 0) {
             string = (char*)realloc(string, sizeof(char) * (i + 2));
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
int ATREIDES_fillUsers(User ** users) {
    char * buffer;
    int  fd ,i = 0, aux, bytes;
		//,
    //User *users;

    //Apertura del fichero
    fd = open("Atreides/users_memory.txt", O_RDONLY);

    if (fd < 0 ) {
      printF("Fitxer de usuaris erroni\n");
      raise(SIGINT);
    } else {
				*users = (User *) malloc(sizeof(User));
        do {

            buffer = ATREIDES_read_until(fd, '-');
            (*users)[i].id  = atoi(buffer);
            if ((*users)[i].id == 0) {
                break;
            }
            
            bytes = read(fd,&aux, sizeof(char));

            strcpy((*users)[i].username,ATREIDES_read_until(fd,'-'));
            bytes = read(fd,&aux, sizeof(char));

            strcpy((*users)[i].postal_code, ATREIDES_read_until(fd,'\n'));

            printf("%d\n",(*users)[i].id );
            printf("%s\n",(*users)[i].username );
            printf("%s\n",(*users)[i].postal_code );

            *users = (User *) realloc(*users, (i+1) * sizeof(User));

            i++;
            free(buffer);
        } while(bytes > 0);

        close(fd);
    }

    return 0;
}

/* ********************************************************************
*
* @Nombre : Main
* @Def : Función principal.
*
********************************************************************* */
int main(int argc, char **argv) {
    int res;
    User * users;

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
    res = ATREIDES_fillUsers(&users);

    if (res == 0 ) {
        printF("Llegit fitxer de usuaris\n");
    }

    raise(SIGINT);

    return 0;
}
