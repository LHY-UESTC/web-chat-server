#gcc 1.server.c  ./common/common.c ./common/wechat.c -I common/ -lpthread -D _D

all:common wechat thread_pool dbmysql serverD
# mysql
LIB_MYSQL = -L/usr/local/mysql -lmysqlclient -lpthread -lz -lm -ldl -lssl -lcrypto

common:../common/common.c
	gcc -c -o common.o $^

wechat:../common/wechat.c
	gcc -c -o wechat.o $^

thread_pool:../common/thread_pool.c
	gcc -c -o thread_pool.o $^ -lpthread

dbmysql:../common/dbmysql.c
	gcc -c -o dbmysql.o $^

server:1.server.c ../common/common.c ../common/wechat.c ../common/thread_pool.c
	gcc -o server $^ -lpthread

serverD:1.server.c ../common/common.c ../common/wechat.c ../common/thread_pool.c ../common/dbmysql.c
	gcc -o server $^ $(LIB_MYSQL) -lpthread -D _D

clean:
	rm -f *.o server