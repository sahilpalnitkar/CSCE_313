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
    }
}

void *worker_function_data(BoundedBuffer *request_buffer, FIFORequestChannel *chan2, HistogramCollection *hc)
{
    /*
        Functionality of the worker threads 
    */
    while(true){

        vector<char> msg_vec = request_buffer->pop();
        char* msg_char = reinterpret_cast<char*>(msg_vec.data());
        MESSAGE_TYPE m = *(MESSAGE_TYPE*) msg_char;
        if (m == DATA_MSG){
            datamsg* data_msg_char = (datamsg*) msg_char;
            chan2->cwrite(msg_char, sizeof(datamsg) + sizeof(msg_char));
            char* buf = chan2->cread();
            double ecg_value = *(double*) buf;
            cout<<"ECG VALUE: "<<ecg_value<<endl;
            hc->update(*data_msg_char, ecg_value);
            hc->print();
        }
        else if (m == QUIT_MSG){
            // MESSAGE_TYPE quitmsg = QUIT_MSG;
            chan2->cwrite(msg_char, sizeof(MESSAGE_TYPE));
            delete chan2;
            cout<<"BREAKING"<<endl;
            break;
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
    int n = 1500;    //default number of requests per "patient"
    int p = 15;     // number of patients [1,15]
    int w = 5000;    //default number of worker threads
    int b = 100;     // default capacity of the request buffer, you should change this default
    int m = MAX_MESSAGE;    // default capacity of the file buffer
    srand(time_t(NULL));
    
    m = 512;
    int pid = fork();
    if (pid == 0){
        // modify this to pass along m
        // char* dataserver_args[] = {"./dataserver", "-m", (char*)m , NULL};
        execl ("dataserver", "dataserver", (char *)NULL);
        // execvp(dataserver_args[0],dataserver_args);
        
    }
    
    vector<FIFORequestChannel> *channel_vec = new vector<FIFORequestChannel>;
    FIFORequestChannel *chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    BoundedBuffer *request_buffer = new BoundedBuffer(b);
    cout<<channel_vec->size()<<endl;
    HistogramCollection *hc = new HistogramCollection();
    // Histogram h();
    vector<thread> *producer =  new vector<thread>;
    vector<thread> *consumer = new vector<thread>;
    


    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */
    for (int i = 1; i < p+1; i++){
        Histogram *h = new Histogram(10, -2, 2);
        hc->add(h);
        thread *patient_thread = new thread(patient_function_data, i, n, request_buffer, h, chan);
        producer->push_back(move(*patient_thread));
    }

    queue<vector<char>> que = request_buffer->getqueue();
    vector<char> msg_char_vec = que.front();
    for (int i = 0; i < w; i++){
        MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
        char* msg = new char[sizeof(MESSAGE_TYPE)];
        *msg = (char)newchannel;
        chan->cwrite(msg, sizeof(MESSAGE_TYPE));
        
        string channel_name = chan->cread();
        // cout<<channel_name<<endl;
        FIFORequestChannel *chan2  = new FIFORequestChannel(channel_name, FIFORequestChannel::CLIENT_SIDE);
    
        thread *worker_thread = new thread(worker_function_data, request_buffer,chan2, hc);
        consumer->push_back(move(*worker_thread));
    }
    cout<<"CONSUMER SIZE IS: "<< producer->size()<<endl;


    /* Join all threads here */
    for(int i = 0; i < producer->size(); i++){
        producer->at(i).join();
    }

    for(int i = 0; i < w; i ++){
        MESSAGE_TYPE q = QUIT_MSG;
        char* buf = (char*) &q;
        vector<char> push_msg1 = vector<char>(buf, buf+ sizeof(datamsg));
        request_buffer->push(push_msg1);
        
    }
    
    for(int i = 0; i < consumer->size(); i++){
        consumer->at(i).join();
        hc->print();
    }
    // gettimeofday (&end, 0);
    cout<<"PRINTING THE HISTOGRAM"<<endl;
    hc->print ();
    // int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    // int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    // cout << "Took " << secs << " seconds and " << usecs << " micor seconds" << endl;

    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}

