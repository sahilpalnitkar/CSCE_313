#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <iostream>


using namespace std;


void sig_handler(int sig){	
	int status;
	//cout<<"handler pid: "<<getpid()<<endl;
    if (wait(&status) < 0){
        perror("Error while waiting for child");
        exit(errno);
    }
    if (WIFEXITED(status)){
        cout<<"child exited"<<endl;
    }

}


int main(){
 	signal (SIGCHLD, sig_handler);
	if (fork()== 0){ // child process
	    cout<<"child process pid is: "<<getpid()<<endl;
		exit(0);
	}
	else {// parent process
	    cout<<"parent process pid is: "<<getpid()<<endl;
	    
		while (true);
		cout<<"after while loop"<<endl;
	}

}