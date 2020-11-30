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
		char sendMsg[MSG_LENGTH];
    int bytesMsgToSend;
	    while(1){
        fgets(sendMsg,MSG_LENGTH,stdin);
        sendMsg[strlen(sendMsg)-1] = '\0';
        if(strlen(sendMsg) == 0){
          printf("Please enter an message!\n");
          continue;
        }
        
        bytesMsgToSend = sizeof(char)*strlen(sendMsg);

        // SENDING MESSAGE LENGTH BYTES
        if (write (sd,&bytesMsgToSend,sizeof(int)) <= 0){
          perror ("[CLIENT]Eroare la write() bytes spre server.\n");
          return errno;
        }

        // SENDING THE MESSAGE
        if (write (sd,&sendMsg,bytesMsgToSend) <= 0){
          perror ("[CLIENT]Eroare la write() message spre server.\n");
          return errno;
        }

        bzero(sendMsg,bytesMsgToSend);
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
        printf("Am primit de la server %s cu size %d\n",msgReceived,bytesReceived);
        bzero(msgReceived,bytesReceived);
   		}
		exit(0);
  }

  close (sd);
}
