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
#define MSG_LENGTH 100

#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;
int ON=1;

typedef struct Client{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
	char ID[2];
}Client;

Client clients[10];
int connectedClients = -1;

static void *client_handler(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int main (){
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
  
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
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

static void *client_handler(void * arg){		
	struct Client tdL; 
	tdL = *((struct Client*)arg);	
	char msg[MSG_LENGTH];
	while(read (tdL.cl, &msg,sizeof(char)*MSG_LENGTH) > 0){
		char msg_to_send[100];
		sprintf(msg_to_send,"[%d]%s",clients[tdL.idThread].idThread,msg);

		for(int i = 0 ; i <= connectedClients ; i++){
			if(clients[i].idThread == tdL.idThread) continue;
			if (write (clients[i].cl, &msg_to_send, sizeof(char)*MSG_LENGTH) <= 0){
				printf("[Client %d] ",tdL.idThread);
				perror ("Eroare la write() catre client.\n");
			}
		}
		fflush (stdout);		 
		// pthread_detach(pthread_self());		
	}
	/* am terminat cu acest client, inchidem conexiunea */
	close ((intptr_t)arg);
	return(NULL);	
};