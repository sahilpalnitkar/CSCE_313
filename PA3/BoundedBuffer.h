#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

class BoundedBuffer
{
private:
  	int cap;
  	queue<vector<char>> q;

	/* mutexto protect the queue from simultaneous producer accesses
	or simultaneous consumer accesses */
	mutex mtx;

	/* condition that tells the consumers that some data is there */
	condition_variable data_available;
	/* condition that tells the producers that there is some slot available */
	condition_variable slot_available;

public:
	BoundedBuffer(int _cap){
		cap = _cap;

	}
	~BoundedBuffer(){

	}

	void push(vector<char> data){
		// mtx.lock();
		unique_lock<std::mutex> lck(mtx);
		slot_available.wait(lck, [this]{return q.size() < cap;});
		q.push(data);
		// cout<<"PUSHED"<<endl;queue<vector<char>> getqueue()
		cout<<"SIZE AFTER PUSH: "<<q.size()<<endl;
		// cout<<"queue size is "<< q.size()<<endl;
		data_available.notify_one();
		// mtx.unlock();
		lck.unlock();

	}

	vector<char> pop(){
		// mtx.lock();
		// cout<<"tRYING TO pOP"<<endl;
		unique_lock<std::mutex> lck(mtx);
		data_available.wait(lck, [this]{return q.size() > 0;});
		vector<char> temp = q.front();
		// cout<<"POPPED"<<endl;
		q.pop();
		cout<<"SIZE AFTER POP: "<<q.size()<<endl;
		// cout<<"queue size is "<< q.size()<<endl;
		slot_available.notify_one();
		return temp;
		lck.unlock();
	}

	int size(){
		mtx.lock();
		return q.size();
		mtx.unlock();
	}

	queue<vector<char>> getqueue(){
		return q;
	}
		
};

#endif /* BoundedBuffer_ */
