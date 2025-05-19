CC = clang
CFLAGS = -Wall -g

OBJS_MANAGER = main.o treasure_funcs.o
OBJS_HUB = treasure_hub.o
OBJS_SCORE = calculate_score.o

all: treasure_manager treasure_hub calculate_score

treasure_manager: $(OBJS_MANAGER)
	$(CC) $(CFLAGS) -o treasure_manager $(OBJS_MANAGER)

treasure_hub: $(OBJS_HUB)
	$(CC) $(CFLAGS) -o treasure_hub $(OBJS_HUB)

calculate_score: $(OBJS_SCORE)
	$(CC) $(CFLAGS) -o calculate_score $(OBJS_SCORE)

main.o: main.c treasure.h
	$(CC) $(CFLAGS) -c main.c

treasure_funcs.o: treasure_funcs.c treasure.h
	$(CC) $(CFLAGS) -c treasure_funcs.c

treasure_hub.o: treasure_hub.c treasure.h
	$(CC) $(CFLAGS) -c treasure_hub.c

calculate_score.o: calculate_score.c
	$(CC) $(CFLAGS) -c calculate_score.c

clean:
	rm -f *.o treasure_manager treasure_hub calculate_score
