#include <iostream>
#include <thread>
#include <stdlib.h>
#include "Semaphore.h"
#include <vector>
#include <unistd.h>

using namespace std;
#define A_COUNT 1
#define B_COUNT 2
#define C_COUNT 1

Semaphore mtx(1);

int a_counter = 0;
int b_counter = 0;
int c_counter = 0;

Semaphore a_done(0);
Semaphore b_done(0);
Semaphore c_done(C_COUNT);

void function_a(int thread){

    c_done.P();
    mtx.P();
    cout<<"This is thread A: "<<thread<<endl;
    a_counter++;
    if (a_counter == A_COUNT){
			for (int i = 0; i < B_COUNT; i++){
                a_done.V();
			}
		a_counter = 0;
	}

    mtx.V();
    
}

void function_b(int thread){
    a_done.P();
    mtx.P();
    cout<<"This is thread B: "<<thread<<endl;
    b_counter++;
    if (b_counter == B_COUNT){
			for (int i = 0; i < C_COUNT; i++){
                b_done.V();
			}
		b_counter = 0;
	}
    mtx.V();
}

void function_c(int thread){
    b_done.P();
    mtx.P();
    cout<<"This is thread C: "<<thread<<endl;
    c_counter++;
    if (c_counter == C_COUNT){
			for (int i = 0; i < A_COUNT; i++){
                c_done.V();
			}
		c_counter = 0;
	}
    mtx.V();
}



int main (){
	vector<thread> threads_a;
	vector<thread> threads_b;
	vector<thread> threads_c;

	for (int i=0; i< 100; i++)
		threads_a.push_back(thread (function_a, i+1));

	for (int i=0; i< 300; i++)
		threads_b.push_back(thread (function_b, i+ 1));

	for (int i=0; i< 300; i++)
		threads_c.push_back(thread (function_c, i+ 1));

	
	for (int i=0; i<threads_a.size (); i++)
		threads_a [i].join();
	
	for (int i=0; i<threads_b.size (); i++)
		threads_b [i].join();
	
	for (int i=0; i<threads_c.size (); i++)
		threads_c [i].join();
	
}

