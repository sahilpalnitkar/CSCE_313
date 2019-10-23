#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"
#include<vector>
#include <fcntl.h>

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
            hc->update(data_msg_char, x); //change update function
        }
        else{
            chan2->cwrite(msg_char,sizeof(MESSAGE_TYPE));
            delete chan2;
            break;
        }

    }
    
}

void patient_function_file(BoundedBuffer *request_buffer, string filename, FIFORequestChannel *chan)
{
    /* What will the patient threads do? */
    //datamsg d ();
    filemsg file = filemsg(0,0);
    char* buf = new char[sizeof(filemsg) + filename.length() + 1];
    memcpy(buf, (char*) &file, sizeof(filemsg));
    strcpy(buf + sizeof(filemsg), filename.c_str());
    int request = chan->cwrite(buf, sizeof(filemsg)+filename.length()+1);

    int file_length = *(int*)chan->cread(&request);
    cout<<"FILE LENGTH: "<<file_length<<endl;
    int file_request_limit = file_length/MAX_MESSAGE;
    int file_request_counter = 0;
    int offset = 0;
    int remaining_file_length = MAX_MESSAGE*file_request_limit;

    while(file_request_counter < file_request_limit){
        // cout<<"FILE LENGTH: "<<file_length<<endl;
        filemsg request_1 = filemsg(offset,MAX_MESSAGE);
        char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
        memcpy(buf2, (char*)&request_1, sizeof(filemsg));
        strcpy(buf2 + sizeof(filemsg), filename.c_str()); 
        vector<char> vec_char (buf2, buf2 + sizeof(filemsg) + filename.length() + 1);
        request_buffer->push(vec_char);
        file_request_counter++;
        cout<<"OFFSET IN PATIENT: "<<offset<<endl;
        offset += MAX_MESSAGE;
    }
    // cout<<"AFTER STRCPY"<<endl;

    if (file_length - offset > 0){
        // cout<<"AFTER STRCPY"<<endl;
        cout<<"OFFSET REMAINDER: "<<offset<<endl;
        cout<<"OFFSET SUBTRACTION: "<<file_length-offset<<endl;
        
        filemsg request_1 = filemsg(offset, file_length - offset);
        cout<<"FILEMSG LENGTH IS: "<<request_1.length<<endl;
        char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];

        memcpy(buf2, (char*)&request_1, sizeof(filemsg));
        strcpy(buf2 + sizeof(filemsg), filename.c_str());          
        cout<<"GETTING LAST CHUNK"<<endl;

        vector<char> vec_char (buf2, buf2 + sizeof(filemsg) + filename.length() + 1);

        request_buffer->push(vec_char);
        char* buf3 = buf2 + sizeof(filemsg);
        filemsg check = *(filemsg*) buf3;
        cout<<"BIFFER LENGTH IS: "<<check.length<<endl;
    }
}


void worker_function_file(BoundedBuffer *request_buffer, string filename, FIFORequestChannel *chan2){
    // ofstream outputFile("received");
    cout<<"AFTER STRCPY"<<endl;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int f_write = open("x.csv", O_APPEND | O_RDWR | O_CREAT , mode);

    while(true)
    {

        vector<char> msg_vec = request_buffer->pop();
        // cout<<"HERE3"<<endl;

        char* msg_char = new char[sizeof(filemsg) + filename.length() + 1];
        msg_char = reinterpret_cast<char*>(msg_vec.data());

        if (*(MESSAGE_TYPE*) msg_char == FILE_MSG)
        {
            // string outfilename = "y"+filename;
            // int f_write = open(outfilename.c_str(), ios::out | ios::binary | ios::app);
            char* filemsg_buf = msg_char + sizeof(filemsg);
            filemsg file_msg = *(filemsg*) filemsg_buf;
            // cout<<"OFFSET IS: "<<file_msg.length<<endl;
            int request2 = chan2->cwrite(msg_char, sizeof(filemsg)+ filename.length() + 1);
            char* ret_buf = (char*)chan2->cread(&request2);
            lseek(f_write, file_msg.offset, SEEK_SET);
            cout<<"OFFSET IS: "<<f_write<<endl;
            if (file_msg.length < MAX_MESSAGE){
                write(f_write, ret_buf, file_msg.length);
            }
            else{
                write(f_write, ret_buf, MAX_MESSAGE);
            }
        }
        else{
            chan2->cwrite(msg_char,sizeof(MESSAGE_TYPE));
            delete chan2;
            break;
        }

    }
}
int main(int argc, char *argv[])
{
    int n = 15000;          //default number of requests per "patient"
    int p = 15;             // number of patients [1,15]
    int w = 1;              //default number of worker threads
    int b = 50;             // default capacity of the request buffer, you should change this default
    int m = MAX_MESSAGE;    // default capacity of the file buffer
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
    string filename = "1.csv";
    /* Start all threads here */

    // for (int i = 1; i < p+1; i++){
    //     Histogram *h = new Histogram(10, -2, 2);
    //     hc->add(h);
    //     thread *patient_thread = new thread(patient_function_data, i, n, &request_buffer);
    //     producer->push_back(patient_thread);
    // }
    thread *patient_thread = new thread(patient_function_file, &request_buffer, filename, chan);
    // patient_thread->join();

    string outfilename = "y"+filename;
    // for (int i = 0; i < w; i++){
        MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
        chan->cwrite((char*) &newchannel, sizeof(MESSAGE_TYPE));
        char* buf = chan->cread();       
        FIFORequestChannel *chan2  = new FIFORequestChannel(buf, FIFORequestChannel::CLIENT_SIDE);
        // thread *worker_thread = new thread(worker_function_file, &request_buffer, filename, chan2);
        worker_function_file(&request_buffer, filename, chan2);
        MESSAGE_TYPE q = QUIT_MSG;
        chan2->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
        delete chan2;
        // consumer->push_back(worker_thread);
    // }
    // for (int i = 0; i < w; i++){
    //     MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
    //     chan->cwrite((char*) &newchannel, sizeof(MESSAGE_TYPE));
    //     char* buf = chan->cread();        
    //     FIFORequestChannel *chan2  = new FIFORequestChannel(buf, FIFORequestChannel::CLIENT_SIDE);
    //     thread *worker_thread = new thread(worker_function_data, &request_buffer,chan2, hc);
    //     consumer->push_back(worker_thread);
    // }
    
    // for(int i = 0; i < p; i++){
    //     producer->at(i)->join();
    // }

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

    // MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}
