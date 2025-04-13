#include "treasure.h"

int add_treasure(const char *hunt_id, Treasure *t) {
    char path[256];
    snprintf(path, sizeof(path), "%s", hunt_id);

    // create hunt
    mkdir(path, 0755); //only owner can rwx

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("add_treasure: open");
        return -1;
    }

    if (write(fd, t, sizeof(Treasure)) != sizeof(Treasure)) {
        perror("add_treasure: write");
        close(fd);
        return -1;
    }

    close(fd);

    //logging
    char msg[128];
    snprintf(msg, sizeof(msg), "added treasure ID %d by user %s", t->treasure_id, t->username);
    log_operation(hunt_id, msg);
    create_symlink(hunt_id);

    return 0;
}

int list_treasures(const char *hunt_id) {
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, TREASURE_FILE);

    struct stat st;
    if (stat(file_path, &st) != 0) {
        perror("list_treasures: stat");
        return -1;
    }

    printf("hunt: %s\n", hunt_id);
    printf("file size: %ld bytes\n", st.st_size);
    printf("last modified: %s", ctime(&st.st_mtime));

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("list_treasures: open");
        return -1;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d, user: %s, latitude: %.5f, longitude: %.5f, value: %d\n",
               t.treasure_id, t.username, t.latitude, t.longitude, t.value);
    }

    close(fd);
    return 0;
}

int view_treasure(const char *hunt_id, int treasure_id) {
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("view_treasure: open");
        return -1;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.treasure_id == treasure_id) {
            printf("treasure ID: %d\nuser: %s\nlatitude: %.5f\nlongitude: %.5f\nclue: %s\nvalue: %d\n",
                   t.treasure_id, t.username, t.latitude, t.longitude, t.clue, t.value);
            close(fd);
            return 0;
        }
    }

    printf("treasure with ID %d not found in hunt %s.\n", treasure_id, hunt_id);
    close(fd);
    return 1;
}

int remove_treasure(const char *hunt_id, int treasure_id) {
    char file_path[256], tmp_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, TREASURE_FILE);
    snprintf(tmp_path, sizeof(tmp_path), "%s/temp.dat", hunt_id);

    int in_fd = open(file_path, O_RDONLY);
    int out_fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (in_fd < 0 || out_fd < 0) {
        perror("remove_treasure: open");
        return -1;
    }

    Treasure t;
    int found = 0;
    while (read(in_fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.treasure_id == treasure_id) {
            found = 1;
            continue;
        }
        write(out_fd, &t, sizeof(Treasure));
    }

    close(in_fd);
    close(out_fd);

    if (!found) {
        printf("Treasure ID %d not found in hunt %s.\n", treasure_id, hunt_id);
        unlink(tmp_path);
        return 1;
    }

    // replace old file
    rename(tmp_path, file_path);

    char msg[128];
    snprintf(msg, sizeof(msg), "removed treasure ID %d", treasure_id);
    log_operation(hunt_id, msg);
    return 0;
}

int remove_hunt(const char *hunt_id) {
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, TREASURE_FILE);
    unlink(file_path);

    snprintf(file_path, sizeof(file_path), "%s/%s", hunt_id, LOG_FILE);
    unlink(file_path);

    if (rmdir(hunt_id) != 0) { //removes directory
        perror("remove_hunt: rmdir");
        return -1;
    }

    char symlink_name[256];
    snprintf(symlink_name, sizeof(symlink_name), "logged_hunt-%s", hunt_id);
    unlink(symlink_name);

    char msg[128];
    snprintf(msg, sizeof(msg), "removed hunt %s", hunt_id);

    return 0;
}


void log_operation(const char *hunt_id, const char *message) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, LOG_FILE);

    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("log_operation: open");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", t); //formats the local time t and puts it in 
                                                                                            //timestamp as buffer
    dprintf(fd, "%s %s\n", timestamp, message);  //writes to fd
    close(fd);
}

void create_symlink(const char *hunt_id) {
    char target[256];
    snprintf(target, sizeof(target), "%s/%s", hunt_id, LOG_FILE);

    char link_name[256];
    snprintf(link_name, sizeof(link_name), "logged_hunt-%s", hunt_id);

    // to avoid errors for existing symlinks
    unlink(link_name);
    
    if (symlink(target, link_name) == -1) {
        perror("create_symlink: symlink");
    }
}


