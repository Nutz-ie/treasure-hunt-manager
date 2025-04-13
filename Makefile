CC = gcc
CFLAGS = -Wall -g
OBJS = main.o treasure_funcs.o

treasure_manager: $(OBJS)
	$(CC) $(CFLAGS) -o treasure_manager $(OBJS)

clean:
	rm -f *.o treasure_manager
