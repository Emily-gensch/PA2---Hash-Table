CC = gcc
CFLAGS = -Wall -pthread

chash: chash.o  init_table.o destroy_table.o insert.o delete.o search.o print_table.o jenkins_hash.o
	$(CC) $(CFLAGS) -o chash chash.o  init_table.o destroy_table.o insert.o delete.o search.o print_table.o jenkins_hash.o

chash.o: chash.c chash.h
	$(CC) $(CFLAGS) -c chash.c

init_table.o: init_table.c chash.h
	$(CC) $(CFLAGS) -c init_table.c

destroy_table.o: destroy_table.c chash.h
	$(CC) $(CFLAGS) -c destroy_table.c

insert.o: insert.c chash.h
	$(CC) $(CFLAGS) -c insert.c

delete.o: delete.c chash.h
	$(CC) $(CFLAGS) -c delete.c

search.o: search.c chash.h
	$(CC) $(CFLAGS) -c search.c

print_table.o: print_table.c chash.h
	$(CC) $(CFLAGS) -c print_table.c

jenkins_hash.o: jenkins_hash.c chash.h
	$(CC) $(CFLAGS) -c jenkins_hash.c

clean:
	rm -f *.o chash