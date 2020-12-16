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
#include "../services/functions.h"
#include "../database/database.h"

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

void login(int cl,char* username,char* password,int* isLogged){
  char msgToSend[100];
  printf("processing [username]%s [pass]%s\n",username,password);
  if( username == NULL || password == NULL){
    strcpy(msgToSend,"Please enter an username and password !");
  } else {
    // *(isLogged) = 1;
    login_command(username,password);
    strcpy(msgToSend,"Login succesfully!");
  }  
  send_msg(cl,msgToSend);
}

void _register(int cl,char* username,char* password){
  char msgToSend[100];
  printf("processing [username]%s [pass]%s\n",username,password);
  if( username == NULL || password == NULL){
    strcpy(msgToSend,"Please enter an username and password !");
  } else {
    if(insert_new_user(username,password) == 0){
      strcpy(msgToSend,"Register succesfully! Please login now");
    } else {
      strcpy(msgToSend,"Something went wrong! Please try again.");
    }
  }
  send_msg(cl,msgToSend);
}


void command_handler(int cl , char msgReceived[],int* isLogged){
  char msgToSend[100];

    if(*(isLogged) == 1){
      strcpy(msgToSend,"Incorrect login command!\n");
      send_msg(cl,msgToSend);
    } else {
      if(strcmp(getNWord(msgReceived,1),"login") == 0){
        login(cl,getNWord(msgReceived,2),getNWord(msgReceived,3),isLogged);
      } else if(strcmp(getNWord(msgReceived,1),"register") == 0){
        _register(cl,getNWord(msgReceived,2),getNWord(msgReceived,3));
      } else {
        strcpy(msgToSend,"Please login or register!\n");
        send_msg(cl,msgToSend);
      }

    }

}

#endif