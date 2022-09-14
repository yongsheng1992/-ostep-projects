# include<stdio.h>
# include<string.h>
# include<stdlib.h>

char *EXIT = "exit";

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

char **parse(char *input) {
    while (input && input != "\0") {
        strsep(&input, "&");
    }
    
}



int main(int argc, char *argv[]) {
    char *line;
    char *trimed;
    size_t len = 0;
    do {
        printf("wish> ");
        getline(&line, &len, stdin);
        trimed = strtrim(line);
        free(trimed);
    } while (strcmp(trimed, EXIT) != 0);

    return 0;
}
