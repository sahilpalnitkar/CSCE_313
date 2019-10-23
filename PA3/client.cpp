#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
#include<vector>

using namespace std;


void patient_function_data(int patient_number, int num_points, BoundedBuffer *request_buffer)
{
    /* What will the patient threads do? */
    //datamsg d ();
    for (int i = 0; i < num_points; i++)
    {
        datamsg* data1 = new datamsg (patient_number, i*0.004, 1);
        vector<char> vec ((char*) data1, (char*) data1 + sizeof(datamsg));
        request_buffer->push(vec);
    }
    
}

    /* What will the patient threads do? */


void worker_function_data(BoundedBuffer *request_buffer,FIFORequestChannel *chan2, HistogramCollection* hc)
{
    /*
		Functionality of the worker threads	
    */
    //FIFORequestChannel chan ("worker", FIFORequestChannel::CLIENT_SIDE);
   
       while(true)
       {
        vector<char> msg_vec = request_buffer->pop();
        char* msg_char = reinterpret_cast<char*>(msg_vec.data());
        
        if (*(MESSAGE_TYPE*) msg_char == DATA_MSG)
        {
            chan2->cwrite(msg_char, sizeof(msg_char) + sizeof(datamsg));
            char* buf = chan2->cread();
            datamsg data_msg_char = *(datamsg*) msg_char;
            double x = *(double*) buf;
            //cout << x;
            hc->update(data_msg_char, x); //change update function
            // hc->print();
        }
        else
        {
            chan2->cwrite(msg_char,sizeof(MESSAGE_TYPE));
            delete chan2;
            break;
        }
    
       }
    
}
int main(int argc, char *argv[])
{
    int n = 15000;    //default number of requests per "patient"
    int p = 15;     // number of patients [1,15]
    int w = 250;    //default number of worker threads
    int b = 50; 	// default capacity of the request buffer, you should change this default
	int m = MAX_MESSAGE; 	// default capacity of the file buffer
    srand(time_t(NULL));
    
    
    int pid = fork();
    if (pid == 0){
        // modify this to pass along m
        execl ("dataserver", "dataserver", (char *)NULL);

    }

    FIFORequestChannel* chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    BoundedBuffer request_buffer(b);
    HistogramCollection* hc = new HistogramCollection();
    vector<thread*> *producer = new vector<thread*>;
    vector<thread*> *consumer = new vector<thread*>;
    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */

    for (int i = 1; i < p+1; i++){
        Histogram *h = new Histogram(10, -2, 2);
        hc->add(h);
        thread *patient_thread = new thread(patient_function_data, i, n, &request_buffer);
        producer->push_back(patient_thread);
    }


    for (int i = 0; i < w; i++){
        MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
        chan->cwrite((char*) &newchannel, sizeof(MESSAGE_TYPE));
        char* buf = chan->cread();        
        FIFORequestChannel *chan2  = new FIFORequestChannel(buf, FIFORequestChannel::CLIENT_SIDE);
        thread *worker_thread = new thread(worker_function_data, &request_buffer,chan2, hc);
        consumer->push_back(worker_thread);
    }
    
    for(int i = 0; i < p; i++){
        producer->at(i)->join();
    }

    for(int i = 0; i < w; i++){
        MESSAGE_TYPE q = QUIT_MSG;
        vector<char> push_msg1((char*) &q, (char*) &q + sizeof(MESSAGE_TYPE));
        request_buffer.push(push_msg1);        
    }
    
    for(int i = 0; i < w; i++){
        consumer->at(i)->join();
        // hc->print();
    }

	/* Join all threads here */
    gettimeofday (&end, 0);
	hc->print ();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micor seconds" << endl;

    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}
