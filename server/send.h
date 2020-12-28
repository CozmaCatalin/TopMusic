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
    char msg[1000]; sprintf(msg,"%s",msgToSend);
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
  char msgToSend[1000];
  printf("processing [username]%s [pass]%s\n",username,password);
  if( username == NULL || password == NULL){
    strcpy(msgToSend,"Please enter an username and password !");
  } else {
    int user_id = login_command(username,password);
    if(user_id != 0){
      strcpy(msgToSend,"Login succesfully!");
      *(isLogged) = user_id;
    } else {
      strcpy(msgToSend,"User doesn't exist!");
    }
  }  
  send_msg(cl,msgToSend);
}

void _register(int cl,char* username,char* password){
  char msgToSend[1000];
  printf("processing [username]%s [pass]%s\n",username,password);
  if( username == NULL || password == NULL){
    strcpy(msgToSend,"Please enter an username and password !");
  } else {
    if(insert_new_user(username,password) == 1){
      strcpy(msgToSend,"Register succesfully! Please login now");
    } else {
      strcpy(msgToSend,"Something went wrong! Please try again.");
    }
  }
  send_msg(cl,msgToSend);
}

void song_insert(int cl,char *name,char *description,char *artist, char *link){
  char msgToSend[1000];
  printf("processing [%s] [%s] [%s] [%s] \n",name,description,artist,link);
  if( name == NULL || description == NULL || artist == NULL || link == NULL ){
    strcpy(msgToSend,"Incoret insert ! => insert [name] [description] [artist] [link] !");
  } else {
    if(insert_song(name,description,artist,link) == 1){
      strcpy(msgToSend,"Song inserted!");
    } else {
      strcpy(msgToSend,"Something went wrong! Please try again.");
    }
  }
  send_msg(cl,msgToSend);
}

void all_music(){
  get_all_music();
}

void top_music(){

}

void vote_song(int cl,char* song_id,int* user_id){
  char msgToSend[1000];
  if(song_id == NULL){
    strcpy(msgToSend,"Incorect command ! => vote [song_id]");
  } else if(find_song(song_id) == 0){
    strcpy(msgToSend,"Music doesn't exist!");
  } else if(user_can_vote(user_id) == 0){
    strcpy(msgToSend,"You can't vote ! Contact an admin.");
  } else if(unique_vote(user_id,song_id) == 0){
    strcpy(msgToSend,"You already voted this song!");
  } else {
    vote(user_id,song_id);
    strcpy(msgToSend,"Song voted!");
  }
  send_msg(cl,msgToSend);
}

// TODO -> Verify if user exists
void dissable_vote(int cl,char* user_id){
  char msgToSend[1000];
  if(user_id == NULL){
    strcpy(msgToSend,"Incorect command ! => dissable [user_id]");
  } else if(vote_dissable(user_id) == 1){
    sprintf(msgToSend,"User %s can't vote now!",user_id);
  }
  send_msg(cl,msgToSend);
}

// TODO -> Verify if user exists
void enable_vote(int cl,char* user_id){
  char msgToSend[1000];
  if(user_id == NULL){
    strcpy(msgToSend,"Incorect command ! => enable [user_id]");
  } else if(vote_enable(user_id) == 1){
    sprintf(msgToSend,"User %s can vote now!",user_id);
  }
  send_msg(cl,msgToSend);
}

void command_handler(int cl , char msgReceived[], int* isLogged){
    char msgToSend[1000];

    // === LOGGED COMMANDS === ///
    if(*(isLogged) != 0){
      if(strcmp(msgReceived,"help") == 0){
        strcpy(msgToSend,"Available commands: \n insert [name] [description] [artist] [link] (insert a new song in database \n)");
        send_msg(cl,msgToSend);
      } else if(strcmp(getNWord(msgReceived,1),"insert") == 0){
        char *name = getNWord(msgReceived,2); char *description = getNWord(msgReceived,3);
        char *artist = getNWord(msgReceived,4); char *link = getNWord(msgReceived,5);
        song_insert(cl,name,description,artist,link);
      } else if(strcmp(getNWord(msgReceived,1),"music") == 0){
        all_music();
      } else if(strcmp(getNWord(msgReceived,1),"top") == 0){

      } else if(strcmp(getNWord(msgReceived,1),"vote") == 0){
        vote_song(cl,getNWord(msgReceived,2),isLogged);
      } else if(strcmp(getNWord(msgReceived,1),"dissable") == 0){
        dissable_vote(cl,getNWord(msgReceived,2));
      } else if(strcmp(getNWord(msgReceived,1),"enable") == 0){
        enable_vote(cl,getNWord(msgReceived,2));
      } else {
        strcpy(msgToSend,"Incorrect login command!\n");
        send_msg(cl,msgToSend);
      }

    // === NON LOGGED COMMANDS === ///
    } else {
      if(strcmp(getNWord(msgReceived,1),"login") == 0){
        login(cl,getNWord(msgReceived,2),getNWord(msgReceived,3),isLogged);
      } else if(strcmp(getNWord(msgReceived,1),"register") == 0){
        _register(cl,getNWord(msgReceived,2),getNWord(msgReceived,3));
      } else if(strcmp(msgReceived,"help") == 0){
        strcpy(msgToSend,"Available commands: \n login [username] [password] \n register [username] [password])\0");
        send_msg(cl,msgToSend);
      } else {
        strcpy(msgToSend,"Please login or register!\n");
        send_msg(cl,msgToSend);
      }
    }

}

#endif