all: pollcli pollserv
CC = gcc
CFLAGS = -Wall -g
pollcli: pollcli.c socketio.o
	$(CC) $(CFLAGS) pollcli.c socketio.o -o pollcli 
pollserv: pollserv.c socketio.o
	$(CC) $(CFLAGS) pollserv.c socketio.o -o pollserv 
socketio.o: socketio.c 
	$(CC) $(CFLAGS) -c socketio.c -o socketio.o 

clean:
	rm -f *.o $(BIN)
