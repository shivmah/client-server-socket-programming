all:
	gcc  server.c -lpthread -g  -o server
	gcc client.c -g -o client
	./kill_ipcs.sh
	
