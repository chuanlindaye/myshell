#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXARGS 64

int tokenize(char *line, char **argv);
int run_builtin(char **argv);
void execute(char ** argv);

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    while(1){
        printf("mysh>");
        fflush(stdout);
        
        if(getline(&line,&cap, stdin)== -1){
            putchar('\n');
            break;
        }

        char *argv[MAXARGS];
        int argc = tokenize(line,argv);
        if(argc == 0) continue;
        if(run_builtin(argc)) continue;

        execute(argc);
    }
    
    free(line);
    return 0;
}

int tokenize(char *line, char ** argv){
    int num = 0;
    if(line == NULL) return 0;
    argv[num] = strtok(line," \t\n");
    while(argv[num] != NULL){
        num++;
        argv[num] = strtok("NULL"," \t\n");
    }
    if(num >= MAXARGS) num = -1;
    return num;
}

int run_builtin(char **argv){
    
}
