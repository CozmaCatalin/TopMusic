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

#ifndef HELPERS_SERVER
#define HELPERS_SERVER

void send_msg(int cl,char msgToSend[]){
    int bytesMsgToSend = sizeof(char)*strlen(msgToSend);
    char msg[100]; sprintf(msg,"%s",msgToSend);
    printf("SENDING %s cu size %d\n",msgToSend,bytesMsgToSend);
    
    // SENDING MESSAGE LENGTH BYTES
    if (write (cl,&bytesMsgToSend,sizeof(int)) <= 0){
      perror ("[SERVER]Eroare la write() bytes spre client.\n");
      bzero(msgToSend,bytesMsgToSend);
      return;
    }

    // SENDING THE MESSAGE
    if (write (cl,&msg,bytesMsgToSend) <= 0){
      perror ("[SERVER]Eroare la write() message spre client.\n");
      bzero(msgToSend,bytesMsgToSend);
      return;
    }
    bzero(msgToSend,bytesMsgToSend);
    fflush(stdout);
}

void login(int cl){
  char msgToSend[100];
  strcpy(msgToSend,"Login command!");
  send_msg(cl,msgToSend);
}

void command_handler(int cl , char msgReceived[]){
  char msgToSend[100];
    if(strcmp(msgReceived,"login") == 0){
      login(cl);
    } else {
      strcpy(msgToSend,"Incorrect command!");
      send_msg(cl,msgToSend);
    }
}

#endif