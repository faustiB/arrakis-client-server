// #include "fremen.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <fcntl.h>
// #include <signal.h>
// //Define
// #define printF(x) write(1, x, strlen(x))
//
// //prototipo
// void RsiControlC(void);
// 
//
// Config configuration;
//
// /* ********************************************************************
// *
// * @Nombre : RsiControlC
// * @Def : Rutina asociada a la interrupción Control+C.
// *
// ********************************************************************* */
// void RsiControlC(void){
//
//   //Printamos el mensaje de adiós al sistema
// 	printF("\n\nDesconnectat d’Atreides. Dew!\n\n");
//
//   free(configuration.ip);
//   free(configuration.directory);
// 	//Terminamos el programa enviándonos a nosotros mismos el signal de SIGINT
// 	raise(SIGKILL);
// }
//
// /* ********************************************************************
// *
// * @Nombre : Main
// * @Def : Función principal.
// *
// ********************************************************************* */
// int main(int argc, char **argv) {
//
//     int fd, command = 0;
//
//     //Check de argumentos de entrada
//     if (argc != 2) {
// 		printF("Error, falta el nom de fitxer de configuracio.\n");
//         return -1;
//     }
//     //Apertura del fichero
//     fd = open(argv[1], O_RDONLY, 0666);
//
//     //Lectura del fichero de configuración y cierre de su file descriptor.
//     configuration = fillConfiguration(configuration, fd);
//     close(fd);
//
//     //Assignación del signal de CtrlC a nuestra función
//     signal(SIGINT, (void*)RsiControlC);
//
//
//
//
//     //Inicialización de fremen.
// 	  printF("Benvingut a Fremen\n");
//     while (command == 0 || command == 1 ) {
// 		    command = promptChoice();
//     }
//
//     //Exit de fremen con la palabra logout.
// 	  raise(SIGINT);
//
//     return 0;
// }
