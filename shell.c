#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>

#define MAXARGS 64

int tokenize(char *line, char **argv);
int run_builtin(char **argv);
void execute(char ** argv);
int my_cd(char ** argv);

int main(void)
{
    while(1){
    char *line = readline("mysh> ");
    if(line == NULL){
        putchar('\n');
        break;
    }

    if(*line !='\0'){
        add_history(line);
    }

    char *argv[MAXARGS];
    int argc = tokenize(line,argv);
    if(argc > 0 && !run_builtin(argv)){
        execute(argv);
    }

    free(line);
    }
    
    return 0;
}

int tokenize(char *line, char ** argv){
    int num = 0;
    char *p = line;

    while(*p){
        //skip blank between words
        while(*p == ' ' || *p == '\t' || *p == '\n'){p++;}
        if(*p == '\0') break;

        //left a position for NULL at tail
        if(num == MAXARGS - 1){
            fprintf(stderr, "mysh: too many arguments\n");
            return -1;
        }

        //prepare a word: start is the beginning, w is the writing pointer
        char *start = p;
        char *w = p;
        int in_single = 0;
        int in_double = 0;

        //scan a word by character
        while(*p){
            if(in_single){
                if(*p == '\''){in_single = 0;p++;}
                else *w++ = *p++;
            }
            else if(in_double){
                if(*p == '"'){in_double = 0; p++;}
                else if(*p == '\\' && (p[1] == '"' || p[1] == '\\')){
                    p++;
                    *w++ = *p++;
                }
                else *w++ = *p++;
            }
            else {
                if(*p == ' ' || *p == '\t' || *p == '\n') break;
                if(*p == '\''){in_single = 1; p++;}
                else if(*p == '"'){in_double = 1;p++;}
                else if(*p == '\\' &&p[1] != '\0'){
                    p++;
                    *w++ = *p++;
                } 
                else *w++ = *p++;
            }   
        } 
        //quote doesn't close
        if(in_single || in_double){
            fprintf(stderr, "mysh: unmatched quote\n");
            return -1;
        }

        //skip the dividor,write the terminator
        if(*p != '\0') p++;
        *w = '\0';
        argv[num++] = start;  
    }

    argv[num] = NULL;
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
