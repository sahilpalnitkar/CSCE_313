#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
using namespace std;


void *patient_function_data(int patient_number, int request_count, BoundedBuffer *request_buffer, Histogram *h, FIFORequestChannel *chan2)
{
    /* What will the patient threads do? */
    for (double time_of_ecg = 0.000; time_of_ecg < request_count * 0.004; time_of_ecg = time_of_ecg + 0.004){
    // for(int i = 0; i < request_count; i++){
            datamsg data1 = datamsg (patient_number, time_of_ecg, 1);
            char* data_buf = (char*) &data1;
            vector<char> push_msg1 = vector<char>(data_buf, data_buf+ sizeof(datamsg));
            request_buffer->push(push_msg1);
            // chan2->cwrite(data_buf, sizeof(datamsg));
            // char* buf = chan2->cread();
            // cout<<"ECG VALUE: "<<*(double*) buf<<endl;
    }
}

void *worker_function_data(BoundedBuffer *request_buffer, FIFORequestChannel *chan2, HistogramCollection hc)
{
    /*
		Functionality of the worker threads	
    */
    // // FIFORequestChannel* worker_chan = new FIFORequestChannel("worker", FIFORequestChannel::CLIENT_SIDE);
    // MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
    // char* buf = (char*) newchannel;
    // chan->cwrite(buf, sizeof(MESSAGE_TYPE));
    // string channel_name = chan->cread();
    // cout<<channel_name<<endl;
    // FIFORequestChannel *chan2  = new FIFORequestChannel("worker", FIFORequestChannel::CLIENT_SIDE);
    // while (request_buffer->size() > 0){
    while(true){

        vector<char> msg_vec = request_buffer->pop();
        char* msg_char = reinterpret_cast<char*>(msg_vec.data());
        MESSAGE_TYPE m = *(MESSAGE_TYPE*) msg_char;
        if (m == DATA_MSG){
            datamsg* data_msg_char = (datamsg*) msg_char;
            chan2->cwrite((char*) data_msg_char, sizeof(datamsg) + sizeof(data_msg_char));
            char* buf = chan2->cread();
            double ecg_value = *(double*) buf;
            cout<<"ECG VALUE: "<<ecg_value<<endl;
            hc.update(*data_msg_char, ecg_value);
            hc.print();
        }
        else{
            MESSAGE_TYPE quitmsg = QUIT_MSG;
            chan2->cwrite((char*) &quitmsg, sizeof(MESSAGE_TYPE));
            delete chan2;
        }

        // }
    }
}


void thread_test(int m){
    for (int i = 0; i < 10; i++){
        cout<<"from thread" << m << ", i = " << i <<endl;
    }
	
}


int main(int argc, char *argv[])
{
    int n = 150;    //default number of requests per "patient"
    int p = 15;     // number of patients [1,15]
    int w = 1;    //default number of worker threads
    int b = 50; 	// default capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	// default capacity of the file buffer
    srand(time_t(NULL));
    
    m = 512;
    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
        // char* dataserver_args[] = {"./dataserver", "-m", (char*)m , NULL};
        execl ("dataserver", "dataserver", (char *)NULL);
        // execvp(dataserver_args[0],dataserver_args);
        
    }
    
	FIFORequestChannel *chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    BoundedBuffer *request_buffer = new BoundedBuffer(b);
    for (int i = 0; i < w; i++){
        // MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
    // char* buf = (char*) newchannel;
    // chan->cwrite(buf, sizeof(MESSAGE_TYPE));
    // string channel_name = chan->cread();
    // cout<<channel_name<<endl;
    // FIFORequestChannel *chan2  = new FIFORequestChannel("worker", FIFORequestChannel::CLIENT_SIDE);
    }
	HistogramCollection hc;
    // Histogram h();
	vector<thread> *producer =  new vector<thread>;
    vector<thread> *consumer = new vector<thread>;
    for (int i = 1; i < p+1; i++){
        Histogram *h = new Histogram(10, -2, 2);
        hc.add(h);
        thread *patient_thread = new thread(patient_function_data, i, n, request_buffer, h, chan);
        producer->push_back(move(*patient_thread));
    }

    queue<vector<char>> que = request_buffer->getqueue();
    vector<char> msg_char_vec = que.front();
    for (int i = 0; i < w; i++){
        thread *worker_thread = new thread(worker_function_data, request_buffer, chan, hc);
        consumer->push_back(move(*worker_thread));
    }
    cout<<"CONSUMER SIZE IS: "<< producer->size()<<endl;

    for(int i = 0; i < producer->size(); i++){
        producer->at(i).join();
    }
    for(int i = 0; i < consumer->size(); i++){
        consumer->at(i).join();
    }

    // for (thread d)
    // patient_function_data(1, n, request_buffer);

    // char* data_msg_char = reinterpret_cast<char*>(msg_char_vec.data());
    // chan->cwrite(data_msg_char, sizeof(data_msg_char));
    // char* buf = chan->cread();
    // double ecg_value = *(double*) buf;
    // // h.update(ecg_value);
    // cout<<ecg_value<<endl;
    // worker_function_data(request_buffer);
    // cout<<request_buffer->size()<<endl;
    // for (int i = 0; i < w; i++){
    //     thread worker_thread(worker_function_data, request_buffer, hc);
    //     consumer.push_back(move(worker_thread));
    // }

    // patient_function_data(1, 1000, request_buffer);
	// cout<<&request_buffer.size()<<endl;
	
    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */
    // thread thread1 (thread_test, 1);
    // thread thread2 (thread_test, 2);
    // thread thread3 (thread_test, 3);
    // thread2.join();
    // thread3.join();
    // thread1.join();


	/* Join all threads here */
    // gettimeofday (&end, 0);
    cout<<"PRINTING THE HISTOGRAM"<<endl;
	hc.print ();
    // int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    // int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    // cout << "Took " << secs << " seconds and " << usecs << " micor seconds" << endl;

    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}

