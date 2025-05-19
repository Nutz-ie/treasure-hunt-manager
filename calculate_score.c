#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treasure.h"


typedef struct UserScore {
    char username[USERNAME_LEN];
    int total;
    struct UserScore *next;
} UserScore;

UserScore* find_or_add_user(UserScore **head, const char *username) {
    UserScore *curr = *head;
    while (curr) {
        if (strcmp(curr->username, username) == 0)
            return curr;
        curr = curr->next;
    }

    UserScore *new_node = malloc(sizeof(UserScore));
    strcpy(new_node->username, username);
    new_node->total = 0;
    new_node->next = *head;
    *head = new_node;
    return new_node;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {    
        fprintf(stderr, "Usage: %s <hunt_dir>\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], TREASURE_FILE);
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    UserScore *scores = NULL;
    Treasure t;


    while (fread(&t, sizeof(Treasure), 1, fp) == 1) {
        UserScore *us = find_or_add_user(&scores, t.username);
        us->total += t.value;

    }

    fclose(fp);

    for (UserScore *curr = scores; curr; curr = curr->next) {
        printf("%s: %d\n", curr->username, curr->total);
    }

    while (scores) {
        UserScore *tmp = scores;
        scores = scores->next;
        free(tmp);
    }

    return 0;
}

