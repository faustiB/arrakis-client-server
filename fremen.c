#include "fremen.h"

void RsiControlC(void);
Config configuration;

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
* @Nombre : fremen_readUntilIntro
* @Def : Función para leer hasta un intro.
*
********************************************************************* */
char *fremen_readUntilIntro(int fd, char caracter, int i) {
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
* @Nombre : fremen_fillConfiguration
* @Def : Función para leer el fichero de configuración, y devolverlo en nuestro struct.
*
********************************************************************* */
Config fremen_fillConfiguration(char * argv) {
    char caracter = ' ', *cadena = NULL;
    int i = 0, fd;
    Config c;

    //Apertura del fichero
    fd = open(argv, O_RDONLY);

    if (fd < 0 ) {
      printF("Fitxer de configuració erroni\n");
      raise(SIGINT);

    } else {
      cadena = fremen_readUntilIntro(fd, caracter, i);
      c.seconds_to_clean = atoi(cadena);
      free(cadena);

      c.ip = fremen_readUntilIntro(fd, caracter, i);

      cadena = fremen_readUntilIntro(fd, caracter, i);
      c.port = atoi(cadena);
      free(cadena);

      c.directory = fremen_readUntilIntro(fd, caracter, i);
      close(fd);
    }
    //Lectura del fichero de configuración y cierre de su file descriptor.
    //configuration = fremen_fillConfiguration(configuration, fd);




    return c;
}


/* ********************************************************************
*
* @Nombre : fremen_freeMemory
* @Def : Función para liberar la memoria.
*
********************************************************************* */

void fremen_freeMemory(char * command, char * command_lower, char ** command_array){
  free(command);
  free(command_lower);
  free(command_array);
}


/* ********************************************************************
*
* @Nombre : fremen_checkNumberOfWords
* @Def : Función para comprobar el número de parametros para el comando.
*
********************************************************************* */
int fremen_checkNumberOfWords (char *command, int words) {

	if (strcmp(command, "login") == 0) {
		if (words == 3) {
			printF("Comanda OK.\n");
			return 0;
		} else if (words < 3) {
			printF("Comanda KO. Falta paràmetres\n");
		} else {
			printF("Comanda KO. Massa paràmetres\n");
		}
		return 1;
	}

	else if (strcmp(command, "search") == 0) {
		if (words == 2) {
			printF("Comanda OK.\n");
			return 0;
		} else if (words < 2) {
			printF("Comanda KO. Falta paràmetres\n");
		} else {
			printF("Comanda KO. Massa paràmetres\n");
		}
		return 1;
	}

	else if (strcmp(command, "send") == 0) {
		if (words == 2) {
			printF("Comanda OK.\n");
			return 0;
		} else if (words < 2) {
			printF("Comanda KO. Falta paràmetres\n");
		} else {
			printF("Comanda KO. Massa paràmetres\n");
		}
		return 1;
	}

	else if (strcmp(command, "photo") == 0) {
		if (words == 2) {
			printF("Comanda OK.\n");
			return 0;
		} else if (words < 2) {
			printF("Comanda KO. Falta paràmetres\n");
		} else {
			printF("Comanda KO. Massa paràmetres\n");
		}
		return 1;
	}

	else if (strcmp(command, "logout") == 0) {
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
* @Nombre : fremen_promptChoice
* @Def : Función para tratar el comando que se introduce
*
********************************************************************* */
int fremen_promptChoice() {
    char *command=NULL, *command_lower = NULL;
	  char *(*command_array);
    int i = 0, num_of_words = 0, isok = 0;

    //Lectura por pantalla del comando y tratado para quedarnos con una cadena
	  printF("$ ");
    command = fremen_readUntilIntro(STDIN_FILENO, '\n', 0);

    //Intro vacío
	  if (command[0] == '\0') {
        free(command);
		    return 0;
	  }


    //Tratamiento pasar cadena a minúscula
    command_lower = strdup(command);
    for (size_t i = 0; command_lower[i] != '\0' ; ++i) {
        command_lower[i] = tolower((unsigned char) command_lower[i]);
    }
    //Tratamiento del comando para separar la cadena de entrada.
    command_array = (char **) malloc(sizeof(char*));
	  command_array[i] = strtok(command_lower ," ");


    while(command_array[i] != NULL) {
		      num_of_words++;
          command_array = realloc(command_array,(num_of_words + 1) * sizeof(char*));
          command_array[++i] = strtok(NULL," ");
   	}

	  //Checkeo del número de parametros.
	  isok = fremen_checkNumberOfWords(command_array[0], num_of_words);
    //Comando custom OK
  	if (isok == 0) {
        if (strcmp(command_array[0],"logout") == 0) {
          fremen_freeMemory(command,command_lower,command_array);
          return 3;
        }
        //FUNCIONALIDADES CUSTOM AQUÍ 





        fremen_freeMemory(command,command_lower,command_array);
  		  return 1;
    //Comando custom KO, por parametros
  	} else if (isok == 1) {
      fremen_freeMemory(command,command_lower,command_array);
  		  return 0;

	//Cuando el  valor retornado sea 2, se abrirá un fork para ejecutar la comanda contra el sistema
		} else {
		    pid_t  pid;
        //Creación del fork
     	  if ((pid = fork()) < 0) {
          fremen_freeMemory(command,command_lower,command_array);
          printF("Error al crear el Fork\n");
          exit(1);
     	  }

    //Entrada al código del hijo.
		else if (pid == 0) {
      //Ejecución del comando +  tratamiento si ha ido mal
      if (execvp(command_array[0], command_array) < 0) {
        fremen_freeMemory(command,command_lower,command_array);
				printF("Error al executar la comanda!\n");
        exit(1);
			}
      //Código del padre
    } else {
            //Espera a la ejecución del comando.
          	wait(NULL);

            //liberacion de memoria
            fremen_freeMemory(command,command_lower,command_array);

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
int main(int argc, char **argv) {

    int command = 0;

    //Check de argumentos de entrada
    if (argc != 2) {
		printF("Error, falta el nom de fitxer de configuracio.\n");
        return -1;
    }

    //Assignación del signal de CtrlC a nuestra función
    signal(SIGINT, (void*)RsiControlC);
    //Lectura del fichero de configuración y cierre de su file descriptor.
    configuration = fremen_fillConfiguration(argv[1]);


    //Inicialización de fremen.
	  printF("Benvingut a Fremen\n");
    while (command == 0 || command == 1 ) {
		    command = fremen_promptChoice();
    }

    //Exit de fremen con la palabra logout.
	  raise(SIGINT);

    return 0;
}
