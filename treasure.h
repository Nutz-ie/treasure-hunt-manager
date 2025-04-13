#ifndef TREASURE_H //checks if treasure.h doesn't already exist
#define TREASURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for stuff like read
#include <fcntl.h> //for open w args
#include <sys/stat.h> //for mkdir
#include <time.h> //duh
#include <errno.h>

#define USERNAME_LEN 30
#define CLUE_LEN 100
#define TREASURE_FILE "treasures.dat"
#define LOG_FILE "logged_hunt"

typedef struct {
    int treasure_id;
    char username[USERNAME_LEN];
    double latitude;
    double longitude;
    char clue[CLUE_LEN];         
    int value;
} Treasure;

void log_operation(const char *hunt_id, const char *message);
void create_symlink(const char *hunt_id);

int add_treasure(const char *hunt_id, Treasure *t);
int list_treasures(const char *hunt_id);
int view_treasure(const char *hunt_id, int treasure_id);
int remove_treasure(const char *hunt_id, int treasure_id);
int remove_hunt(const char *hunt_id);

#endif

