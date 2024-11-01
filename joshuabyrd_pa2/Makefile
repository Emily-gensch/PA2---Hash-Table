CC = gcc
CFLAGS = -Wall -pthread

chash: chash.o hash_operations.o
	$(CC) $(CFLAGS) -o chash chash.o hash_operations.o

chash.o: chash.c chash.h
	$(CC) $(CFLAGS) -c chash.c

hash_operations.o: hash_operations.c chash.c
	$(CC) $(CFLAGS) -c hash_operations.c

clean:
	rm -f *.o chash