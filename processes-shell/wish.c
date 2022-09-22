#include "wish.h"

void strtrim(char **str) {
    int len;
    char *head, *tail;
    head = *str;
    tail = head + strlen(*str) - 1;
    
    while (isspace((unsigned char)*head)) {
        head++;
    }
    while (tail > head && isspace((unsigned char)*tail)) {
        tail--;
    }
    tail[1] = '\0';
}

int check_redirection(char *str) {
    char *token = strsep(&str, ">");
    if (token == NULL) {
        return 0;
    }
    int file_cnt = 0;
    while ((token = strsep(&str, " ")) != NULL) {
        file_cnt++;
    }
    
    if (file_cnt > 1) {
        return 1;
    }
    return 0;
}

void append(list_t* list, command_t* cmd) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if (list->head == NULL) {
        list->head = node;
        list->tail = list->head;
        return;
    }
    list->tail->next = node;
    list->tail = list->tail->next;
}

list_t* parse(char *str) {
    char *token;
    list_t* list = (list_t *)malloc(sizeof(list));

    while ((token = strsep(&str, "&&")) != NULL) {
        if (check_redirection(token) != 0) {
            return NULL;
        }
        command_t *cmd = parse_command(token);
        append(list, cmd);
    }
    return list; 
}


command_t* parse_command(char *str) {
    char *argc;
    char *argv;
    argc = strsep(&str, " ");
    argv = strsep(&str, ">");
    command_t* cmd;
    cmd = (command_t*)malloc(sizeof(command_t));
    cmd->argv = (char **)malloc(sizeof(char *) * 2);

    cmd->argc = argc;
    cmd->argv[0] = argv;
    cmd->argv[1] = NULL;
    cmd->output = str;
    return cmd;
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
        // char *cmd = "/usr/bin/ls";
        // list *l = parse(command);
        // errno = execv(cmd, argvs);
        // free(argvs);
        // exit(errno);
    }
}


int main(int argc, char *argv[]) {
    char *line;
    size_t  len = 0;

    do {
        printf("wish> ");
        getline(&line, &len, stdin);
        strtrim(&line);
        printf("%s\n", line);
    } while (strcmp(line, EXIT) != 0);
    return 0;
}
