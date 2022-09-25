#include "wish.h"

char *strtrim(char *str) {
    if (str == NULL) {
        return NULL;
    }
    int len;
    char *head, *tail;
    head = str;
    tail = head + strlen(str) - 1;
    
    while (isspace((unsigned char)*head)) {
        head++;
    }
    while (tail > head && isspace((unsigned char)*tail)) {
        tail--;
    }
    tail[1] = '\0';
    return head;
}

int check_redirection(char *str) {
    char *s = strdup(str);
    char *saved = s;
    // char *s = str;
    char *token = NULL;
    token = strsep(&s, ">");
    if (s == NULL) {
        return 0;
    }
    int file_cnt = 0;
    s = strtrim(s);
    while ((token = strsep(&s, " ")) != NULL) {
        file_cnt++;
    }

    if (file_cnt > 1) {
        return 1;
    }
    free(saved);
    return 0;
}

void append(list_t* list, command_t* cmd) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->cmd = cmd;
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
    list->head = NULL;
    list->tail = NULL;

    while ((token = strsep(&str, "&&")) != NULL) {
        if (check_redirection(token) != 0) {
            return list;
        }
        command_t *cmd = parse_command(token);

        if (cmd->output != NULL && cmd->fd < 1) {
             write(STDERR_FILENO, error_msg, strlen(error_msg));
             return list;
        }
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
    cmd->output = NULL;
    cmd->argc = strtrim(argc);
    cmd->argv[0] = strtrim(argv);
    cmd->argv[1] = NULL;
    cmd->output = strtrim(str);
    if (cmd->output != NULL) {
        cmd->fd = open(cmd->output, O_CREAT|O_RDWR|O_TRUNC, 0666);
    }
    return cmd;
}

char* check_access(command_t *cmd) {
    char *p = path;
    char *dir;
    char *abs_path = NULL;

    while ((dir = strsep(&p, ";")) != NULL) {
        abs_path = (char *)malloc(sizeof(char) * (strlen(dir) + 1 + strlen(cmd->argc)));
        strcat(abs_path, dir);
        strcat(abs_path, "/");
        strcat(abs_path, cmd->argc);

        if (access(abs_path, X_OK) == 0) {
            break;
        } else {
            free(abs_path);
            abs_path = NULL;
        }
    }
    return abs_path;
}

void run(char* str) {
    list_t* list = parse(str);
    if (list->head == NULL) {
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    pid_t pid;
    int status = 0;

    char *abs_path = check_access(list->head->cmd);
    if (abs_path == NULL) {
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }

    pid = fork();
    if (pid == 0) {
        int errno = run_command(abs_path, list->head->cmd);
        exit(errno);
    }

    waitpid(pid, &status, 0);
    if (list->head->cmd->fd > 0) {
        close(list->head->cmd->fd);
    }
}

int run_command(char* abs_path, command_t *cmd) {
    int errno = 0;
    char** argvs = (char**)malloc(sizeof(char*) * 3);
    argvs[0] = cmd->argc;
    argvs[1] = cmd->argv[0];
    argvs[2] = cmd->argv[1];
    if (cmd->fd > 0) {
        dup2(cmd->fd, STDOUT_FILENO);
    }
    errno = execv(abs_path, argvs);
    return errno;
}

int main(int argc, char *argv[]) {
    char *line;
    size_t  len = 0;
    list_t* list;

    do {
        printf("wish> ");
        getline(&line, &len, stdin);
        char *str = strtrim(line);
        if (strcmp(str, EXIT) == 0) {
            break;
        }
        run(str);
    } while (1);
    return 0;
}
