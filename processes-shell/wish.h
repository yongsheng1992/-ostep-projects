#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>

char *EXIT = "exit";
char path[512] = "/usr/bin";
char error_msg[] = "An error has occurred\n";
int MAX_COMMAND = 100;

struct command {
    char* argc;
    char** argv;
    char* output;
    int fd;
};

typedef struct command command_t;

struct node {
    command_t* cmd;
    struct node* next;
};

typedef struct node node_t;

struct list {
    node_t* head;
    node_t* tail;
};

typedef struct list list_t;

void append(list_t* list, command_t *cmd);

char *strtrim(char *str);
list_t *parse(char *str);
command_t *parse_command(char *str);
int check_redirection(char *str);
char *find_cmd(command_t *cmd);
void run(char* str);
int run_command(char *abs_path, command_t *cmd);
void append_path(char **paths, char *path);
void change_dir(char *dir);
void exit();
