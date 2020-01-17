#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

class Semaphore{
private:
    int value;
    mutex m;
    condition_variable cv;
public:
    Semaphore (int _v):value(_v){
        
    }
    void P(){
		unique_lock<mutex> l (m);
		cv.wait (l, [this]{return value > 0;});
        value --;
    }
    void V(){
        unique_lock<mutex> l(m);
        value ++;
        cv.notify_all();
    }
};