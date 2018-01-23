all: selcli selserv
CC = gcc
CFLAGS = -Wall -g
selcli: selcli.c socketio.o
	$(CC) $(CFLAGS) selcli.c socketio.o -o selcli 
selserv: selserv.c socketio.o
	$(CC) $(CFLAGS) selserv.c socketio.o -o selserv 
socketio.o: socketio.c 
	$(CC) $(CFLAGS) -c socketio.c -o socketio.o 

clean:
	rm -f *.o $(BIN)
