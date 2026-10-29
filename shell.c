#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXARGS 64

int tokenize(char *line, char **argv);
int run_builtin(char **argv);
void execute(char ** argv);
int my_cd(char ** argv);

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
        if(run_builtin(argv)) continue;

        execute(argv);
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
        argv[num] = strtok(NULL," \t\n");
    }
    if(num >= MAXARGS) num = -1;
    return num;
}

int run_builtin(char **argv){
    if(strcmp(argv[0],"exit") == 0){
        int code = argv[1] ? atoi(argv[1]) : 0;
        exit(code);
    }
    if(strcmp(argv[0],"cd") == 0){
        my_cd(argv);
        return 1;
    }
    return 0;
}

void execute(char ** argv){
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        return;
    }
    if(pid == 0){//subprocess
        execvp(argv[0],argv);
        perror(argv[0]);//when fail to here
        _exit(127);
    }

    int status;
    waitpid(pid, &status, 0);
}

int my_cd(char **argv){
    const char *dir = argv[1] ? argv[1] : getenv("HOME");

    if(dir == NULL){
        fprintf(stderr, "cd: HOME not set\n");
        return -1;
    }
    if(chdir(dir) != 0){
        perror("cd");
        return -1;
    }
    return 0;
}
