#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <iostream>
#include <semaphore.h>  
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>   


using namespace std; 


class KernelSemaphore{
	string name;
	sem_t* sem;
public:
	KernelSemaphore(string _name, int _init_value){
		name = _name;
		sem = sem_open(_name.c_str(), O_CREAT, 0664, _init_value);
	}
	void P(){
		sem_wait(sem);
	}
	void V(){
		sem_post(sem);
	}
	~KernelSemaphore (){
		sem_close(sem);
	}
};