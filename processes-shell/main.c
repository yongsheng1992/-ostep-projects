#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

char *EXIT = "exit";
char path[512] = "/usr/;/usr/bin/";

char *strtrim(char *target) {
    int len;
    char *head, *tail;
    char *res = malloc(strlen(target));
    char *cursor = res;
    head = target;
    tail = target + strlen(target) - 1;
    
    while (*head <= 32) {
        head++;
    }
    while (*tail <= 32) {
        tail--;
    }
    
    for (;head <= tail ;) {
        *cursor = *head;
        cursor++;
        head++;
    }
    return res;
}

int search(char *command) {

}

char **parse(char *command) {
    char **argv = malloc(256 * sizeof(char *));
    char **p = argv;
    char *head = command;
    char*token;
    while ((token = strsep(&head, " ")) != NULL) {
        *p = token;
        p++;
    }
    return argv;
}


void execute(char *command) {
    pid_t pid;
    int status;
    pid = fork();
    // parent process
    if (pid != 0) {
        waitpid(pid, &status, 0);
        printf("child process %d exit.\n", pid);
    } else { // child process
        int errno;
        char *cmd = "/usr/bin/ls";
        char **argvs = parse(command);
        errno = execv(cmd, argvs);
        free(argvs);
        exit(errno);
    }
}


int main(int argc, char *argv[]) {
    char *line;
    char *command = malloc(sizeof("ls -l"));
    memcpy(command, "ls -l", sizeof("ls -l"));
    size_t len = 0;
    execute(command);
    // do {
    //     printf("wish> ");
    //     getline(&line, &len, stdin);
    //     command = strtrim(line);
    //     execute(command);
    //     // free(trimed);
    // } while (strcmp(command, EXIT) != 0);

    return 0;
}
