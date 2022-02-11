/*************************************************************************
	> File Name: dbmysql.c
	> Author:
	> Mail:
	> Created Time: Fri 03 Dec 2021 07:25:49 PM CST
 ************************************************************************/

#include "head.h"

char mysql_username[256];
char mysql_password[256];
MYSQL *conn;
MYSQL_RES *result;
MYSQL_ROW row;
int section = 0;

int transection() {
	return section;
}
void set_transection(int x) {
	section = x;
}

int ReadMysqlPassword(){
	FILE* f=fopen("mysql.ini","r");
	fscanf(f,"%s",mysql_username);
	fscanf(f,"%s",mysql_password);
	DBG(GREEN"mysql_username:%s,mysql_password:%s\n"NONE,mysql_username,mysql_password);
	fclose(f);
}

int Init(){
	DBG(YELLOW"before initDb\n"NONE);
	int ret=initDb("127.0.0.1","username","password","ssp");
	set_transection(0);
	result=NULL;
	DBG(GREEN"DbManager Init:%d\n"NONE,ret);
	return ret;
}

int initDb(const char* host,const char* user,const char* pswd,const char* db_name) {
	ReadMysqlPassword();
	conn = mysql_init(NULL);
	if(conn == NULL){
		return DB_CONN_INIT_FAIL;
	}
    DBG(GREEN"dbmysql"NONE" : Init:\n");
	conn = mysql_real_connect(conn,host,mysql_username,mysql_password,db_name,0,NULL,0);
	if(conn == NULL){
		return DB_CONN_CONNECT_FAIL;
	}
    DBG(GREEN"dbmysql"NONE" : connect\n");
	return SUCCESS;
}

int GetUsersBegin(){
	if(transection()==1){
		return DB_IS_BUSY;
	}
	set_transection(1);
	int ret=mysql_query(conn,"select * from wechat_user;");
	if(ret){
		DBG(RED"[WARN    ]query fail : %d %s \n"NONE,ret,mysql_error(conn));
		set_transection(0);
		return DB_QUERY_FAIL;
	}
	result = mysql_use_result(conn);
	DBG(GREEN"GetUsersBegin finished\n"NONE);
	return SUCCESS;
}

int GetUsersOneByOne(struct UserInfo* user){
	if(result){
		int col_count = mysql_field_count(conn);
		int row_count = mysql_num_rows(result);
		DBG("[DEBUG   ]row_num %d, col_num %d\n",row_count,col_count);
		while(result){
			row = mysql_fetch_row(result);
			if(row==NULL){
				DBG(RED"[INFO    ]no more user\n"NONE);
				return DB_NO_MORE_DATA;
			}
			if (!strcmp(user->name, row[0])) {
				if (!strcmp(user->passwd, row[1])) return SUCCESS;
				return USER_WRONG_PASSWD;
			}
			DBG("%s\t%s\n",row[0], row[1]);
		}
		row_count = mysql_num_rows(result);
		DBG("[DEBUG   ]row_num %d, col_num %d\n",row_count,col_count);
	}
	return DB_NO_MORE_DATA;
}

int GetUsersEnd(){
	mysql_free_result(result);
	set_transection(0);
	return SUCCESS;
}


int InsertUser(struct UserInfo* user) {
	char insertSql[512];
	sprintf(insertSql, "insert into wechat_user(name,passwd) values('%s','%s');",user->name,user->passwd);
	DBG("[DEBUG   ]insertSql:%s\n",insertSql);
	int ret=mysql_query(conn,insertSql);
	if(ret==0){
		return SUCCESS;
	}else{
		DBG(RED"[WARN    ]insert fail : %d %s \n"NONE,ret,mysql_error(conn));
		return DB_QUERY_FAIL;
	}	
	return SUCCESS;
}

int Db_check_user(struct UserInfo* user) {
	int ans = SUCCESS;
	if (GetUsersBegin() == SUCCESS) {
		int res = GetUsersOneByOne(user);
		if (res == SUCCESS) {
			DBG(BLUE"用户%s密码%s 已存在,注册失败\n"NONE, user->name,user->passwd);
			ans = USER_SIGNUP_FAIL;
		}
		GetUsersEnd();
	}
	return ans;
}