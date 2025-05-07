#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define COMMAND_FILE "monitor_command.txt"
#define BUFFER_SIZE 512

pid_t monitor_pid = -1;
int monitor_running = 0;
int monitor_terminating = 0;

void handle_sigchld(int sig) {
    int status;
    pid_t pid = waitpid(monitor_pid, &status, WNOHANG);
    if (pid > 0) {
        printf("Monitor process %d terminated.\n", pid);
        monitor_running = 0;
        monitor_terminating = 0;
    }
}

void sigusr1_handler(int sig) {
    //does nothing, used to wake up pause()
}

void start_monitor() {
    if (monitor_running) {
        printf("Monitor is already running.\n");
        return;
    }

    monitor_pid = fork();
    if (monitor_pid < 0) {
        perror("fork");
        return;
    }

if (monitor_pid == 0) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &sa, NULL);

        while (1) {
            pause();

            int fd = open(COMMAND_FILE, O_RDONLY);
            if (fd < 0) continue;

            char command[BUFFER_SIZE];
            read(fd, command, BUFFER_SIZE);
            close(fd);

            if (strncmp(command, "LIST_HUNTS", 10) == 0) {
                system("find . -type d -exec sh -c 'echo -n {}: && find {} -name treasures.dat | wc -l' \\;"); //runs a shell command that finds each hunt's
                                                                                                 //treasure.dat file and truncates it to just the directory
            } else if (strncmp(command, "LIST_TREASURES", 14) == 0) {
                char hunt_id[BUFFER_SIZE];
                sscanf(command + 15, "%s", hunt_id);

                char cmd[BUFFER_SIZE];
                snprintf(cmd, sizeof(cmd), "./treasure_manager --list %s", hunt_id);
                system(cmd);
            } else if (strncmp(command, "VIEW_TREASURE", 13) == 0) {
                char hunt_id[BUFFER_SIZE];
                int id;
                sscanf(command + 14, "%s %d", hunt_id, &id);

                char cmd[BUFFER_SIZE];
                snprintf(cmd, sizeof(cmd), "./treasure_manager --view %s %d", hunt_id, id);
                system(cmd);
            } else if (strncmp(command, "STOP", 4) == 0) {
                usleep(1000000); // Delay to test signal wait
                exit(0);
            }
        }
    } else {
        monitor_running = 1;
        printf("Started monitor with PID %d\n", monitor_pid);
    }
}

void send_command_to_monitor(const char *command) {
    if (!monitor_running) {
        printf("Error: Monitor is not running.\n");
        return;
    }
    if (monitor_terminating) {
        printf("Monitor is terminating. Please wait.\n");
        return;
    }
    int fd = open(COMMAND_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open command file");
        return;
    }
    write(fd, command, strlen(command));
    close(fd);
    kill(monitor_pid, SIGUSR1);
}

void stop_monitor() {
    if (!monitor_running) {
        printf("Monitor is not running.\n");
        return;
    }
    send_command_to_monitor("STOP");
    monitor_terminating = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    char input[BUFFER_SIZE];

    while (1) {
        printf("treasure_hub> ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "start_monitor") == 0) {
            start_monitor();

        } else if (strcmp(input, "list_hunts") == 0) {
            send_command_to_monitor("LIST_HUNTS");

        } else if (strncmp(input, "list_treasures", 14) == 0) {
            char hunt_id[BUFFER_SIZE];
            if (sscanf(input + 15, "%s", hunt_id) == 1) {
                char cmd[BUFFER_SIZE];
                snprintf(cmd, sizeof(cmd), "LIST_TREASURES %s", hunt_id);
                send_command_to_monitor(cmd);
            } else {
                printf("Usage: list_treasures <hunt_id>\n");
            }

        } else if (strncmp(input, "view_treasure", 13) == 0) {
            char hunt_id[BUFFER_SIZE];
            int tid;
            if (sscanf(input + 14, "%s %d", hunt_id, &tid) == 2) {
                char cmd[BUFFER_SIZE];
                snprintf(cmd, sizeof(cmd), "VIEW_TREASURE %s %d", hunt_id, tid);
                send_command_to_monitor(cmd);
            } else {
                printf("Usage: view_treasure <hunt_id> <treasure_id>\n");
            }

        } else if (strcmp(input, "stop_monitor") == 0) {
            stop_monitor();

        } else if (strcmp(input, "exit") == 0) {
            if (monitor_running) {
                printf("Error: monitor still running. Stop it first.\n");
            } else {
                break;
            }

        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}

