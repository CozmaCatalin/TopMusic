/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h> 
#include <mysql/mysql.h>
#include "send.h"
#include<signal.h>
#define MSG_LENGTH 100

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[]){
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis
  int nr=0;
  char buf[10];
  pid_t communications;
  printf("MySQL client version: %s\n", mysql_get_client_info());
  /* exista toate argumentele in linia de comanda? */
  if (argc != 3){
      printf ("Sintaxa: %s <SERVER_ADDRESS> <PORT>\n", argv[0]);
      return -1;
  }
  int ok = 1;

  port = atoi(argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
    perror ("Eroare la socket().\n");
    return errno;
  }

  /* familia socket-ului */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1){
      perror ("[client]Eroare la connect().\n");
      return errno;
  }
  
  communications = fork();

  if(communications < 0){
      perror ("[client]Eroare la fork().\n");
      return errno;
  } else if(communications > 0){
	  //--TRIMITEREA MESAJELOR--//
	    while(1){
        commands_handler(sd);
   		}
  } else if(communications == 0){
		//--PRIMIREA MESAJELOR--//
    int bytesReceived;
	  char msgReceived[MSG_LENGTH];
		char receiveMsg[MSG_LENGTH];
	    while(read (sd, &bytesReceived,sizeof(int)) > 0){
        if (read(sd, &msgReceived,bytesReceived) <= 0){
          perror ("[CLIENT]Eroare la read() de la server.\n");
          return errno;
        }
        if(strcmp(msgReceived,"exit")==0){
          kill(getpid(),SIGINT);
          break;
        }
        printf("%s\n",msgReceived);
        bzero(msgReceived,bytesReceived);
   		}
  }

  close (sd);
}
