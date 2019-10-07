#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>


void  ChildProcess();
void  ParentProcess();
char user_input;
void  main()
{
     scanf(" %s",&user_input);
     pid_t  pid;

     pid = fork();
     if (pid == 0) 
     ChildProcess();
          
     else 
     ParentProcess();
     
          
}

void  ChildProcess(void)
{
     
     char *grep_args[] = {"grep", &user_input, "-e", "a.txt", "-c", NULL};
     execvp("grep", grep_args);
     // printf("String %s exists in the file at position %d\n", &user_input, retval);

          

     exit(0);
}

void  ParentProcess(void)
{    
     // printf("Writing to txt file\n");
     int out;
     char *ls_args[] = {"ls","-1", NULL};
     out = open("a.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
     dup2(out, 1);
     close(out);
     execvp("ls", ls_args);    
     wait(NULL);
}