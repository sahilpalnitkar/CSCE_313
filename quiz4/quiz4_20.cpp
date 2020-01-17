#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <iostream>
#include <bits/stdc++.h>	

using namespace std;

void signal_handler (int signo){
	printf ("Got SIGUSR1\n");
}
int main (){
	time_t start, end; 
	time(&start);
	// signal (SIGUSR1, signal_handler); //comment out for b)
	int pid = fork ();
	if (pid == 0){// chilld process
		for (int i=0; i<5; i++){
			kill(getppid(), SIGUSR1);
			sleep (1);
		}
	}
	else{ // parent process
		wait(0);
	}
	time(&end);
	double time_taken = double(end - start); 
    cout << "Time taken by program is : " << fixed << time_taken;
    cout << " sec " << endl; 
}