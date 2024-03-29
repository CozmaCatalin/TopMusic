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
const int MSG_LENGTH = 10000;
#ifndef HELPERS_SERVER
#define HELPERS_SERVER

const char* NORMAL_USER_COMMANDS = "AVAILABLE COMMANDS: \n\nexit (disconnect)\ninsert [name] [description] [artist] [link] [type-1] [type-2] ... [type-n] (insert a new song in database)\ntop (get top music)\ntop [type_id] (get top music by type)\ntypes (get current types)\ninsert_type [name] (insert new type in data base)\ncomment [song_id] [text] (add comment to song)\nvote [song_id] [number] (add vote to a song)\n";
const char* ADMIN_COMMANDS = "AVAILABLE COMMANDS: \n\nexit (disconnect)\ninsert [name] [description] [artist] [link] [type-1] [type-2] ... [type-n] (insert a new song in database)\ntop (get top music)\ntop [type_id] (get top music by type)\ntypes (get current types)\ninsert_type [name] (insert new type in data base)\ncomment [song_id] [text] (add comment to song)\nvote [song_id] [number] (add vote to a song)\nuser_dissable [user_id] (dissable user vote capability)\nuser_enable [user_id] (enable user vote capability)\nsong_dissable [song_id] (song can't be on top)\nsong_enable [song_id] (song can be on top)\nsong_delete [song_id] (delete song)\n";

void send_msg(int cl,char msgToSend[]){
    int bytesMsgToSend = sizeof(char)*strlen(msgToSend);
    char msg[MSG_LENGTH]; sprintf(msg,"%s",msgToSend);
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

void login(int cl,char* username,char* password,int* isLogged, int *isAdmin){
  char msgToSend[MSG_LENGTH];
  printf("processing [username]%s [pass]%s\n",username,password);
  if( username == NULL || password == NULL){
    strcpy(msgToSend,"Please enter an username and password !");
  } else {
    int user_id = login_command(username,password,isAdmin);
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
  char msgToSend[MSG_LENGTH];
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

void song_insert(int cl,char *name,char *description,char *artist, char *link,int idOfTypes[],int numberOfTypes){
  char msgToSend[MSG_LENGTH];
  printf("processing [%s] [%s] [%s] [%s] \n",name,description,artist,link);
  if( name == NULL || description == NULL || artist == NULL || link == NULL ){
    strcpy(msgToSend,"Incoret insert ! => insert [name] [description] [artist] [link] [type-1] [type-2] ... [type-n] !");
  } else if(numberOfTypes == 0){
    strcpy(msgToSend,"Please insert at least 1 type!");
  } else {
    if(insert_song(name,description,artist,link,idOfTypes,numberOfTypes) == 1){
      strcpy(msgToSend,"Song inserted!");
    } else {
      strcpy(msgToSend,"Something went wrong! Please try again.");
    }
  }
  send_msg(cl,msgToSend);
}

void top_music(int cl,char* type){
  char msgToSend[MSG_LENGTH];
  strcpy(msgToSend,get_top_music(type));
  send_msg(cl,msgToSend);
}

void vote_song(int cl,char* song_id,char* value,int* user_id){
  char msgToSend[MSG_LENGTH];
  if(song_id == NULL || value == NULL){
    strcpy(msgToSend,"Incorect command ! => vote [song_id] [value]");
  } else if(atoi(value) < 1 || atoi(value) > 5){
    strcpy(msgToSend,"Value must be between 1 and 5!");
  } else if(find_song(song_id) == 0){
    strcpy(msgToSend,"Music doesn't exist!");
  } else if(user_can_vote(user_id) == 0){
    strcpy(msgToSend,"You can't vote ! Contact an admin.");
  } else if(unique_vote(user_id,song_id) == 0){
    strcpy(msgToSend,"You already voted this song!");
  } else {
    vote(user_id,song_id,value);
    strcpy(msgToSend,"Song voted!");
  }
  send_msg(cl,msgToSend);
}

void comment_song(int cl,char* song_id,char* text,int* user_id){
  char msgToSend[MSG_LENGTH];
  if(song_id == NULL || text == NULL){
    strcpy(msgToSend,"Incorect command ! => comment [song_id] [text]");
  } else if(strlen(text) == 0){
    strcpy(msgToSend,"Text can't be empty!");
  } else if(find_song(song_id) == 0){
    strcpy(msgToSend,"Music doesn't exist!");
  } else {
    comment(user_id,song_id,text);
    strcpy(msgToSend,"Comment added!");
  }
  send_msg(cl,msgToSend);
}

void vote_set(int cl,char* user_id,int vote_case){
  char msgToSend[MSG_LENGTH];
  if(user_id == NULL){
    strcpy(msgToSend,"Incorect command ! => user_enable/user_dissable [user_id]");
  } else if(find_user(user_id) == 0){
    strcpy(msgToSend,"User doesn't exist !");
  } else if(set_vote(user_id,vote_case) == 1){
    if(vote_case == 1){
      sprintf(msgToSend,"User %s can vote now!",user_id);
    } else {
      sprintf(msgToSend,"User %s can't vote now!",user_id);
    }
  }
  send_msg(cl,msgToSend);
}

void song_set(int cl,char* song_id,int song_case){
  char msgToSend[MSG_LENGTH];
  if(song_id == NULL){
    strcpy(msgToSend,"Incorect command ! => song_enable/song_dissable [song_id]");
  } else if(find_song(song_id) == 0){
    strcpy(msgToSend,"Song doesn't exist !");
  } else if(set_song(song_id,song_case) == 1){
    if(song_case == 1){
      sprintf(msgToSend,"Song %s can be on top now!",song_id);
    } else {
      sprintf(msgToSend,"Song %s can't be on top now!",song_id);
    }
  }
  send_msg(cl,msgToSend);
}

void song_delete(int cl,char* song_id){
  char msgToSend[MSG_LENGTH];
  if(song_id == NULL){
    strcpy(msgToSend,"Incorect command ! => song_delete [song_id]");
  } else if(find_song(song_id) == 0){
    strcpy(msgToSend,"Song doesn't exist !");
  } else if(delete_song(song_id) == 1){
    sprintf(msgToSend,"Song %s deleted!",song_id);
  }
  send_msg(cl,msgToSend);
}

void types(int cl){
  char msgToSend[MSG_LENGTH];
  strcpy(msgToSend,get_all_types());
  send_msg(cl,msgToSend);
}

void insert_type(int cl, char* name){
  char msgToSend[MSG_LENGTH];
  if(name == NULL){
    strcpy(msgToSend,"Incorect command ! => insert_type [name]");
  } else if(find_type_db(name) == 1){
    strcpy(msgToSend,"Type already exist !");
  } else {
    insert_type_db(name);
    sprintf(msgToSend,"Type %s inserted!",name);
  }
  send_msg(cl,msgToSend);
}

void command_handler(int cl , char msgReceived[], int* isLogged, int *isAdmin){
    char msgToSend[MSG_LENGTH];
    // === LOGGED COMMANDS === ///
    if(*(isLogged) != 0){
      if(strcmp(msgReceived,"help") == 0){
        if(*(isAdmin) == 1){
          strcpy(msgToSend,ADMIN_COMMANDS);
        } else {
          strcpy(msgToSend,NORMAL_USER_COMMANDS);
        }
        send_msg(cl,msgToSend);
      } else if(strcmp(getNWord(msgReceived,1),"insert") == 0){
        char *name = getNWord(msgReceived,2); char *description = getNWord(msgReceived,3);
        char *artist = getNWord(msgReceived,4); char *link = getNWord(msgReceived,5);
        int numberOfTypes = getNumberOfWords(msgReceived) - 5;
        int idOfTypes[100];
        for(int i = 0 ; i < numberOfTypes ; i++){
          idOfTypes[i] = atoi(getNWord(msgReceived,6+i));
        }
        song_insert(cl,name,description,artist,link,idOfTypes,numberOfTypes);
      } else if(strcmp(getNWord(msgReceived,1),"top") == 0){
        top_music(cl,getNWord(msgReceived,2));
      } else if(strcmp(getNWord(msgReceived,1),"types") == 0){
        types(cl);
      } else if(strcmp(getNWord(msgReceived,1),"insert_type") == 0){
        insert_type(cl,getNWord(msgReceived,2));
      } else if(strcmp(getNWord(msgReceived,1),"comment") == 0){
        comment_song(cl,getNWord(msgReceived,2),getNWord(msgReceived,3),isLogged);
      } else if(strcmp(getNWord(msgReceived,1),"vote") == 0){
        vote_song(cl,getNWord(msgReceived,2),getNWord(msgReceived,3),isLogged);

        // === ADMIN COMMANDS === //
      } else if(strcmp(getNWord(msgReceived,1),"user_dissable") == 0 && *(isAdmin) == 1){
        vote_set(cl,getNWord(msgReceived,2),0);
      } else if(strcmp(getNWord(msgReceived,1),"user_enable") == 0 && *(isAdmin) == 1){
        vote_set(cl,getNWord(msgReceived,2),1);
      } else if(strcmp(getNWord(msgReceived,1),"song_dissable") == 0 && *(isAdmin) == 1){
        song_set(cl,getNWord(msgReceived,2),0);
      } else if(strcmp(getNWord(msgReceived,1),"song_enable") == 0 && *(isAdmin) == 1){
        song_set(cl,getNWord(msgReceived,2),1);
      } else if(strcmp(getNWord(msgReceived,1),"song_delete") == 0 && *(isAdmin) == 1){
        song_delete(cl,getNWord(msgReceived,2));
      } else {
        strcpy(msgToSend,"Command doesn't exist! Type help to see available commands\n");
        send_msg(cl,msgToSend);
      }

    // === NON LOGGED COMMANDS === ///
    } else {
      if(strcmp(getNWord(msgReceived,1),"login") == 0){
        login(cl,getNWord(msgReceived,2),getNWord(msgReceived,3),isLogged,isAdmin);
      } else if(strcmp(getNWord(msgReceived,1),"register") == 0){
        _register(cl,getNWord(msgReceived,2),getNWord(msgReceived,3));
      } else if(strcmp(msgReceived,"help") == 0){
        strcpy(msgToSend,"Available commands:\nlogin [username] [password] \nregister [username] [password])\0");
        send_msg(cl,msgToSend);
      } else {
        strcpy(msgToSend,"Please login or register!\n");
        send_msg(cl,msgToSend);
      }
    }

}

#endif