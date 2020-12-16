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
#include "send.h"

#define MSG_LENGTH 100

#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int ON=1;

typedef struct Client{
	int idThread;
	int cl;
  int isLogged;
  int isAdmin;
}Client;

int connectedClients = -1;

static void *client_handler(void * arg){		
	struct Client tdL; 
	tdL = *((struct Client*)arg);	
	char msgReceived[MSG_LENGTH];
  int bytesReceived;
	while(read (tdL.cl, &bytesReceived,sizeof(int)) > 0){

    if (read(tdL.cl, &msgReceived,bytesReceived) <= 0){
      perror ("[SERVER]Eroare la read() de la client.\n");
      break;
    }
    if(strcmp(msgReceived,"exit") == 0){
      send_msg(tdL.cl,msgReceived);
      break;
    }
    command_handler(tdL.cl,msgReceived,&tdL.isLogged);
    bzero(msgReceived,bytesReceived);
    fflush(stdout);
	}
  printf("[%d]Client disconnected!\n",tdL.idThread);
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
  // close_connection();
  
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
	  td = (struct Client*)malloc(sizeof(struct Client));	
	  td->idThread=connectedClients;
	  td->cl=client;
    td->isLogged = 0;
    td->isAdmin = 0;
	  pthread_create(&th[connectedClients], NULL, &client_handler, td);	      
				
	}   
};		
