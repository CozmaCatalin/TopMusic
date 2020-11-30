/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "../database/database.h"

#define MSG_LENGTH 100

#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int ON=1;

typedef struct Client{
	int idThread;
	int cl;
	char ID[2];
}Client;

Client clients[10];
int connectedClients = -1;

		// for(int i = 0 ; i <= connectedClients ; i++){
    //		sprintf(msg_to_send,"[%d]%s",clients[tdL.idThread].idThread,msg);
		// 	if(clients[i].idThread == tdL.idThread) continue;
		// 	if (write (clients[i].cl, &msg_to_send, sizeof(char)*MSG_LENGTH) <= 0){
		// 		printf("[Client %d] ",tdL.idThread);
		// 		perror ("Eroare la write() catre client.\n");
		// 	}
		// }

static void *client_handler(void * arg){		
	struct Client tdL; 
	tdL = *((struct Client*)arg);	
	char msgReceived[MSG_LENGTH];
  char msgToSend[MSG_LENGTH];
  int bytesMsgToSend;
  int bytesReceived;

	while(read (tdL.cl, &bytesReceived,sizeof(int)) > 0){

    if (read(tdL.cl, &msgReceived,bytesReceived) <= 0){
      perror ("[SERVER]Eroare la read() de la client.\n");
      break;
    }

    printf("[%d bytes]CLIENT SEND-> %s\n",bytesReceived,msgReceived);

    sprintf(msgToSend,"[TEST]%s",msgReceived);
    bytesMsgToSend = sizeof(char)*strlen(msgToSend);
    printf("SENDING %s cu size %d\n",msgToSend,bytesMsgToSend);

    // SENDING MESSAGE LENGTH BYTES
    if (write (tdL.cl,&bytesMsgToSend,sizeof(int)) <= 0){
      perror ("[SERVER]Eroare la write() bytes spre client.\n");
      break;
    }

    // SENDING THE MESSAGE
    if (write (tdL.cl,&msgToSend,bytesMsgToSend) <= 0){
      perror ("[SERVER]Eroare la write() message spre client.\n");
      break;
    }

    bzero(msgReceived,bytesReceived);
    bzero(msgToSend,bytesMsgToSend);

		fflush (stdout);		 
	}

	pthread_detach(pthread_self());		
	close ((intptr_t)arg);
	return(NULL);	

};


int main (){
  struct sockaddr_in server;
  struct sockaddr_in from;	
  int nr;
  int sd;
  int pid;
  pthread_t th[100];

  connect_to_database();
  get_tables();
  close_connection();
  
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
      perror ("[server]Eroare la socket().\n");
      return errno;
  }

  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&ON,sizeof(ON));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
    server.sin_family = AF_INET;	
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);
  
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1){
	perror ("[server]Eroare la bind().\n");
	return errno;
  }

  if (listen (sd, 10) == -1){
      perror ("[server]Eroare la listen().\n");
      return errno;
  }
  
  while (1){
    int client;
    Client * td;   
    int length = sizeof (from);

    printf ("[server]Asteptam la portul %d...\n",PORT);
    fflush (stdout);

    if ((client = accept (sd, (struct sockaddr *) &from, &length)) < 0){
      perror ("[server]Eroare la accept().\n");
      continue;
    }
	    
	  connectedClients++;
	  clients[connectedClients].idThread = connectedClients;
	  clients[connectedClients].cl = client;

	  td= (struct Client*)malloc(sizeof(struct Client));	
	  td->idThread=connectedClients;
	  td->cl=client;
	
	  pthread_create(&th[connectedClients], NULL, &client_handler, td);	      
				
	}   
};		
