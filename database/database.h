#include <mysql/mysql.h>
#include <stdio.h>
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

#ifndef DATABASE
#define DATABASE

const char *SHOW_TABLES = "show tables";


const char *server = "localhost";
const char *user = "root";
const char *password = ""; /* set me first */
const char *database = "topmusic";

MYSQL *conn;

void connect_to_database(){	
	conn = mysql_init(NULL);
	/* Connect to database */
	if (!mysql_real_connect(conn, server, user, password,database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	
}

void close_connection(){
	mysql_close(conn);
}

int insert_new_user(char* username,char* password){
	char s[1000];
	sprintf(s,"INSERT INTO `client` (`user_name`,`password`,`first_name`,`last_name`,`age`,`is_admin`,`token`) VALUES ('%s','%s',' ', ' ' ,'10','0','-');",username,password);
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	return 1;
}

int insert_song(char *name,char *description,char *artist, char *link){
	char s[1000];
	sprintf(s,"INSERT INTO `music` (`name`,`description`,`artist`,`link`,`can_be_on_top`) VALUES ('%s','%s','%s','%s',1);",name,description,artist,link);
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	return 1;
}

int find_song(char *song_id){
	char s[1000];
	sprintf(s,"SELECT * FROM music WHERE id=\"%s\";",song_id);

	if (mysql_query(conn, s)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	MYSQL_RES *result = mysql_store_result(conn);
	printf("select %lld \n",mysql_num_rows(result));

	if(mysql_num_rows(result) == 0){
		return 0;
	}
	return 1;
}


int find_user(char *user_id){
	char s[1000];
	sprintf(s,"SELECT * FROM client WHERE id=\"%s\";",user_id);

	if (mysql_query(conn, s)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	MYSQL_RES *result = mysql_store_result(conn);
	printf("select %lld \n",mysql_num_rows(result));

	if(mysql_num_rows(result) == 0){
		return 0;
	}
	return 1;
}

int vote(int *user_id , char *song_id){
	char s[1000];
	sprintf(s,"INSERT INTO `votes` (`number`,`music_id`,`client_id`) VALUES (1,'%s','%d');",song_id,*(user_id));
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	return 1;
}

int unique_vote(int *user_id , char *song_id){
	char s[1000];
	sprintf(s,"SELECT * FROM votes WHERE client_id=\"%d\" AND music_id=\"%s\";",*(user_id),song_id);
	printf("%s \n",s);	

	if (mysql_query(conn, s)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	MYSQL_RES *result = mysql_store_result(conn);
	if(mysql_num_rows(result) == 0){
		return 1;
	}

	return 0;
}

int set_vote(char* user_id , int vote_case){
	char s[1000];
	sprintf(s,"UPDATE client SET can_vote=%d WHERE id=\"%s\";",vote_case,user_id);
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	return 1;
}

int set_song(char* song_id , int song_case){
	char s[1000];
	sprintf(s,"UPDATE music SET can_be_on_top=%d WHERE id=\"%s\";",song_case,song_id);
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	return 1;
}

int user_can_vote(int *user_id){
	char s[1000];
	sprintf(s,"SELECT can_vote FROM client WHERE id=\"%d\";",*(user_id));
	printf("%s \n",s);	

	if (mysql_query(conn, s)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	MYSQL_RES *result = mysql_store_result(conn);
	if(mysql_num_rows(result) == 0){
		return 0;
	}
	MYSQL_ROW row;
	row = mysql_fetch_row(result);
	mysql_free_result(result);
	printf("CAN VOTE %s\n",row[0]);
	return atoi(row[0]);
}

const char* get_all_music(){
	char s[1000];
	sprintf(s,"SELECT * FROM music;");
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	MYSQL_RES *result = mysql_store_result(conn);
	
    if(mysql_num_rows(result) == 0){
        return "Something went wrong!\n";
    }

	int num_fields = mysql_num_fields(result);    

	MYSQL_ROW row;
	
	char* all_music = malloc(sizeof(char)*10000);

	while ((row = mysql_fetch_row(result))) 
	{ 
		for(int i = 0; i < num_fields-1; i++) 
		{ 
			sprintf(all_music,"%s %s",all_music,row[i]);
		} 
		sprintf(all_music,"%s\n",all_music);
	}
	
	mysql_free_result(result);
	return all_music;
}

const char* get_top_music(){
	char s[1000];
	sprintf(s,"SELECT * FROM music;");
	if (mysql_query(conn, s)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
	  return 0;
  	}
	MYSQL_RES *result = mysql_store_result(conn);
	
    if(mysql_num_rows(result) == 0){
        return "Something went wrong!\n";
    }

	int num_fields = mysql_num_fields(result);    

	MYSQL_ROW row;
	
	char* all_music = malloc(sizeof(char)*10000);

	while ((row = mysql_fetch_row(result))) 
	{ 
		for(int i = 0; i < num_fields-1; i++) 
		{ 
			sprintf(all_music,"%s %s",all_music,row[i]);
		} 
		sprintf(all_music,"%s\n",all_music);
	}
	
	mysql_free_result(result);
	return all_music;
}

int login_command(char* username, char* password, int *isAdmin){	
	char s[1000];
	sprintf(s,"SELECT * FROM client WHERE user_name=\"%s\" AND password=\"%s\";",username,password);
	if (mysql_query(conn, s)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	MYSQL_RES *result = mysql_store_result(conn);
	if(mysql_num_rows(result) == 0){
		return 0;
	}

	MYSQL_ROW row;
	row = mysql_fetch_row(result);
	mysql_free_result(result);
	*(isAdmin) = atoi(row[6]);
	return atoi(row[0]);
}

void get_tables(){
	MYSQL_RES *res;
	MYSQL_ROW row;
	/* send SQL query */
	if (mysql_query(conn, SHOW_TABLES)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
   
	res = mysql_use_result(conn);
	
	/* output table name */
	printf("MySQL Tables in mysql database %s:\n",database);
   
	while ((row = mysql_fetch_row(res)) != NULL)
		printf("%s \n", row[0]);
   
	/* close connection */
	mysql_free_result(res);
}

#endif