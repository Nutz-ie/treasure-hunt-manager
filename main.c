#include "treasure.h"

void print_usage() {
    printf("Usage:\n");
    printf("  treasure_manager --add <hunt_id>\n");
    printf("  treasure_manager --list <hunt_id>\n");
    printf("  treasure_manager --view <hunt_id> <treasure_id>\n");
    printf("  treasure_manager --removetreasure <hunt_id> <treasure_id>\n");
    printf("  treasure_manager --removehunt <hunt_id>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }

    const char *command = argv[1];
    const char *hunt_id = argv[2];

    if (strcmp(command, "--add") == 0) {
        Treasure t;
        printf("enter treasure ID: ");
        scanf("%d", &t.treasure_id);
        printf("enter username (max %d chars): ", USERNAME_LEN - 1);
        scanf("%s", t.username);
        printf("enter latitude: ");
        scanf("%lf", &t.latitude);
        printf("enter longitude: ");
        scanf("%lf", &t.longitude);
        printf("enter clue (max %d chars): ", CLUE_LEN - 1);
        getchar(); // flush newline
        fgets(t.clue, CLUE_LEN, stdin);
        t.clue[strcspn(t.clue, "\n")] = 0; // remove newline
        printf("enter value: ");
        scanf("%d", &t.value);

        return add_treasure(hunt_id, &t);

    } else if (strcmp(command, "--list") == 0) {
        return list_treasures(hunt_id);

    } else if (strcmp(command, "--view") == 0 && argc >= 4) {
        int id = atoi(argv[3]);
        return view_treasure(hunt_id, id);

    } else if (strcmp(command, "--removetreasure") == 0 && argc >= 4) {
        int id = atoi(argv[3]);
        return remove_treasure(hunt_id, id);

    } else if (strcmp(command, "--removehunt") == 0) {
        return remove_hunt(hunt_id);

    } else {
        print_usage();
        return 1;
    }
}

