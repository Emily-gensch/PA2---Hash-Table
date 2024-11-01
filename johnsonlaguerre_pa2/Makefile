OBJS =  pa2-delete.o pa2-threads.o pa2-print.o pa2-search.o \
       pa2-insert.o
CFLAGS = -o -pthread
CC = gcc

chash : $(OBJS)
        $(CC) $(CFLAGS) chash $(OBJS)  
# pa2-delete.o : pa2-delete.c 
        # cc -c pa2-delete.c
# pa2-threads.o : pa2-threads.c
        # cc -c pa2-threads.c
# pa2-print.o : pa2-print.c
        # cc -c pa2-print.c
# pa2-search.o : pa2-search.c  
        # cc -c pa2-search.c
# pa2-insert.o : pa2-insert.c  
        # cc -c pa2-insert.c
clean :
        rm chash $(OBJS)
