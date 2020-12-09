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

#ifndef FUNCTIONS
#define FUNCTIONS

char* getNWord(char sentence[],int n){
  char * pch;
  int i = 0;
  pch = strtok (sentence," ,.-");
  while (pch != NULL)
  {
    i++;
    if( i == n){
        return pch;
    }
    pch = strtok (NULL, " ,.-");
  }
  return "Sentence doesn't have this number of word";
}

#endif