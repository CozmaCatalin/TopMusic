#include <mysql/mysql.h>
#include <stdio.h>


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
	if (!mysql_real_connect(conn, server, user, password, 
                                      database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	
}

void close_connection(){
	mysql_close(conn);
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