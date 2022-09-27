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

list_t *new_list() {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void append(list_t* list, command_t* cmd) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->cmd = cmd;
    list->size = list->size + 1;
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
    list_t* list = new_list();
    while ((token = strsep(&str, "&&")) != NULL) {
        if (strlen(token) == 0) {
            continue;
        }
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
    // print_list(list);
    return list; 
}


command_t* parse_command(char *str) {
    char *argc;
    char *argv;
    str = strtrim(str);
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
    cmd->type = SYSTEM;
    if (strcmp(cmd->argc, CMD_EXIT) == 0|| strcmp(cmd->argc, CMD_CD) == 0 || strcmp(cmd->argc, CMD_PATH) == 0) {
        cmd->type = BUILT_IN;   
    }
    return cmd;
}

char* check_access(command_t *cmd) {
    char *path1 = (char *)malloc(sizeof(char)*strlen(path));
    strcat(path1, path);
    char *p = path1;
    char *dir;
    char *abs_path = NULL;
    while ((dir = strsep(&p, ";")) != NULL) {
        abs_path = (char *)malloc(sizeof(char) * (strlen(dir) + 1 + strlen(cmd->argc)));
        abs_path[0] = '\0';

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
    free(path1);
    return abs_path;
}

void print_list(list_t *list) {
    node_t *node = list->head;
    printf("print list start\n");
    while (node != NULL) {
        printf("%s %s\n", node->cmd->argc, node->cmd->argv[0]);
        node = node->next;
    }
    printf("print list end\n");
}

void run(char* str) {
    list_t* list = parse(str);
    if (list->head == NULL) {
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    node_t *node = list->head;

    while (node != NULL) {
        pid_t pid;
        int status = 0;
        if (node->cmd->type == BUILT_IN) {
            if (strcmp(node->cmd->argc, CMD_CD) == 0) {
                change_dir(node->cmd->argv[0]);
            } else if (strcmp(node->cmd->argc, CMD_PATH) == 0) {
                change_path(node->cmd->argv[0]);
            }
            node = node->next;
            continue;
        }
        char *abs_path = check_access(node->cmd);
        if (abs_path == NULL) {
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            return;
        }
        pid = fork();
        if (pid == 0) {
            run_command(abs_path, node->cmd);
        }
        node = node->next;
    }
    pid_t pid;

    while (1) {
        int ret = wait(NULL);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
    }
    
    node = list->head;
    while (node != NULL) {
        if (node->cmd->fd > 0) {
            close(node->cmd->fd);
        }
        node = node->next;
    }
}

int run_command(char* abs_path, command_t *cmd) {
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

void change_path(char *new_apth) {
    path[0] = '\0';
    if (new_apth == NULL) {
        return;
    }
    char *str = new_apth;
    char *token = NULL;
    while ((token = strsep(&str, " ")) != NULL) {
        strcat(path, token);
        strcat(path, ";");
    }
    path[strlen(path)-1] = '\0';
    return;
}

void change_dir(char *dir) {
    if (chdir(dir) < 0 ) {
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
}

int main(int argc, char *argv[]) {
    char *line;
    size_t  len = 0;
    list_t* list;

    do {
        printf("wish> ");
        getline(&line, &len, stdin);
        char *str = strtrim(line);
        if (strcmp(str, CMD_EXIT) == 0) {
            exit(0);
        }
        run(str);
    } while (1);
    return 0;
}
