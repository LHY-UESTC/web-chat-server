#gcc 1.client.c  ./common/common.c  ./common/wechat_ui.c ./common/wechat_client.c -I common/ -lpthread -lncursesw -D UI

all:common wechat_ui wechat_client clientD

common:../common/common.c
	gcc -c -o common.o $^

wechat_ui:../common/wechat_ui.c
	gcc -c -o wechat_ui.o -I common/ $^

wechat_client:../common/wechat_client.c
	gcc -c -o wechat_client.o $^

client:1.client.c ../common/common.c  ../common/wechat_ui.c ../common/wechat_client.c
	gcc -o client $^ -lpthread -lncursesw -D _D

clientD:1.client.c ../common/common.c  ../common/wechat_ui.c ../common/wechat_client.c
	gcc -o client $^ -lpthread -lncursesw -D UI

clean:
	rm -f *.o client