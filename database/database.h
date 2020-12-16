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
	return mysql_query(conn, s);
}

int login_command(char* username, char* password){	
	char s[1000];
	sprintf(s,"SELECT COUNT(*) FROM client WHERE user_name=\"%s\" AND password=\"%s\";",username,password);
	printf("%s \n",s);	
	if (mysql_query(conn, s)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	return 1;
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