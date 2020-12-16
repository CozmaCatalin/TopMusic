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
#include "../services/functions.h"
#ifndef SEND_CLIENT
#define SEND_CLIENT

void send_msg(int sd, char msgToSend[]){
    int bytesMsgToSend = sizeof(char)*strlen(msgToSend);
    char msg[100]; 
    sprintf(msg,"%s",msgToSend);    

    if(strlen(msg) == 0){
        printf("Please enter an message!\n");
        bzero(msg,bytesMsgToSend);
        return;
    }

    // SENDING MESSAGE LENGTH BYTES
    if (write (sd,&bytesMsgToSend,sizeof(int)) <= 0){
        perror ("[CLIENT]Eroare la write() bytes spre server.\n");
        bzero(msg,bytesMsgToSend);
        return;
    }

    // SENDING THE MESSAGE
    if (write (sd,&msg,bytesMsgToSend) <= 0){
        perror ("[CLIENT]Eroare la write() message spre server.\n");
        bzero(msg,bytesMsgToSend);
        return;
    }

    if(strcmp(msg,"exit")==0){
        bzero(msg,bytesMsgToSend);
        exit(0);
    }
    
     bzero(msg,bytesMsgToSend);
}

void commands_handler(int sd){
    char msgToSend[100];
    fgets(msgToSend,100,stdin);
    msgToSend[strlen(msgToSend)-1] = '\0';
    send_msg(sd,msgToSend);
    bzero(msgToSend,100);
}
#endif