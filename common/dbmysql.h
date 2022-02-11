/*************************************************************************
	> File Name: dbmysql.h
	> Author:
	> Mail:
	> Created Time: Fri 03 Dec 2021 07:23:56 PM CST
 ************************************************************************/

#ifndef _DBMYSQL_H
#define _DBMYSQL_H

#define SUCCESS 0


#define USER_WRONG_PASSWD -500
#define USER_SIGNUP_FAIL -501

#define DB_CONN_INIT_FAIL -600
#define DB_CONN_CONNECT_FAIL -601
#define DB_QUERY_FAIL -602
#define DB_IS_BUSY -603
#define DB_NO_MORE_DATA -604

struct UserInfo {
    char name[20];
    char passwd[20];
};

int transection();
void set_transection(int x);
int Init();
int initDb(const char* host,const char* user,const char* pswd,const char* db_name);
int GetUsersBegin();
int GetUsersOneByOne(struct UserInfo* user);
int GetUsersEnd();
int InsertUser(struct UserInfo* user);
int Db_check_user(struct UserInfo* user);

#endif
