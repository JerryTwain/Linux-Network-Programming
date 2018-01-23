all: semaphore
CC = gcc
CFLAGS = -Wall -g
semaphore: semaphore.c 
	$(CC) $(CFLAGS) semaphore.c -o semaphore -lpthread

clean:
	rm -f *.o $(BIN)
