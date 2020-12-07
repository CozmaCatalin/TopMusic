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

#ifndef SEND_CLIENT
#define SEND_CLIENT

void send_msg(int sd){
    char sendMsg[100];
    fgets(sendMsg,100,stdin);
    sendMsg[strlen(sendMsg)-1] = '\0';
    int bytesMsgToSend = sizeof(char)*strlen(sendMsg);
    
    if(strlen(sendMsg) == 0){
        printf("Please enter an message!\n");
        bzero(sendMsg,bytesMsgToSend);
        return;
    }

    // SENDING MESSAGE LENGTH BYTES
    if (write (sd,&bytesMsgToSend,sizeof(int)) <= 0){
        perror ("[CLIENT]Eroare la write() bytes spre server.\n");
        bzero(sendMsg,bytesMsgToSend);
        return;
    }

    // SENDING THE MESSAGE
    if (write (sd,&sendMsg,bytesMsgToSend) <= 0){
        perror ("[CLIENT]Eroare la write() message spre server.\n");
        bzero(sendMsg,bytesMsgToSend);
        return;
    }

    if(strcmp(sendMsg,"exit")==0){
        bzero(sendMsg,bytesMsgToSend);
        exit(0);
    }
    
     bzero(sendMsg,bytesMsgToSend);
}
#endif