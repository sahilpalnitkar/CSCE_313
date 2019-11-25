#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "NRC.h"
#include<vector>
#include <fcntl.h>
#include <iostream>

using namespace std;


void patient_function_data(int patient_number, int num_points, BoundedBuffer *request_buffer)
{
    /* What will the patient threads do? */
    for (int i = 0; i < num_points; i++)
    {
        datamsg* data1 = new datamsg (patient_number, i*0.004, 1);
        vector<char> vec ((char*) data1, (char*) data1 + sizeof(datamsg));
        request_buffer->push(vec);
    }
    
}

    /* What will the patient threads do? */


void worker_function_data(BoundedBuffer *request_buffer, NRC *chan2, HistogramCollection* hc)
{
    /*
        Functionality of the worker threads 
    */
   
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
            hc->update(data_msg_char.person, x); //change update function
        }
        else{
            chan2->cwrite(msg_char,sizeof(MESSAGE_TYPE));
            delete chan2;
            break;
        }

    }
    
}

void patient_function_file(BoundedBuffer *request_buffer, string filename, string name, string port)
{
    
    NRC* chan = new NRC (name, port);
    /* What will the patient threads do? */
    //datamsg d ();
    filemsg file = filemsg(0,0);
    char* buf = new char[sizeof(filemsg) + strlen(filename.c_str())+ 1];
    memcpy(buf, (char*) &file, sizeof(filemsg));
    strcpy(buf + sizeof(filemsg), filename.c_str());
    int request = chan->cwrite(buf, sizeof(filemsg)+strlen(filename.c_str())+1);

    char* read_buf = chan->cread(&request);
    int file_length = *(int*) read_buf;
    int file_request_limit = file_length/MAX_MESSAGE;
    int file_request_counter = 0;
    int offset = 0;
    int remaining_file_length = MAX_MESSAGE*file_request_limit;
    cout<<"total file size is: "<<file_length<<endl;
    cout<<"total request limig is: "<<file_request_limit<<endl;
    
    string outfilename = "received/y"+filename;

    FILE *fp = fopen(outfilename.c_str(), "w");
        fseek(fp, file_length , SEEK_SET);
        // fputc('\0', fp);
        fclose(fp);

    // delete buf;
    // delete read_buf;
    MESSAGE_TYPE q = QUIT_MSG;
    char* data = (char*) &q;
    chan->cwrite(data, sizeof(MESSAGE_TYPE));
    

    while(file_request_counter < file_request_limit){
        // cout<<"FILE LENGTH: "<<file_length<<endl;
        filemsg request_1 = filemsg(offset,MAX_MESSAGE);
        char* buf2 = new char[sizeof(filemsg)+ strlen(filename.c_str()) + 1];
        memcpy(buf2, (char*)&request_1, sizeof(filemsg));
        strcpy(buf2 + sizeof(filemsg), filename.c_str()); 
        vector<char> vec_char (buf2, buf2 + sizeof(filemsg) + strlen(filename.c_str()) + 1);
        request_buffer->push(vec_char);
        file_request_counter++;
        // cout<<"OFFSET IN PATIENT: "<<offset<<endl;
        offset += MAX_MESSAGE;
        cout<<"Offset is: "<<offset<<endl;  
        // delete buf2;
    }
    // cout<<"AFTER STRCPY"<<endl;

    if (file_length - offset > 0){
        // cout<<"AFTER STRCPY"<<endl;
        cout<<"OFFSET REMAINDER: "<<offset<<endl;
        cout<<"OFFSET SUBTRACTION: "<<file_length-offset<<endl;
        
        filemsg request_1 = filemsg(offset, file_length - offset);
        cout<<"FILEMSG LENGTH IS: "<<request_1.length<<endl;
        char* buf2 = new char[sizeof(filemsg)+ strlen(filename.c_str()) + 1];

        memcpy(buf2, (char*)&request_1, sizeof(filemsg));
        strcpy(buf2 + sizeof(filemsg), filename.c_str());          
        cout<<"GETTING LAST CHUNK"<<endl;

        vector<char> vec_char (buf2, buf2 + sizeof(filemsg) + strlen(filename.c_str()) + 1);

        request_buffer->push(vec_char);
        // delete buf2;
        
    }
    // FILE *fp1 = fopen(outfilename.c_str(), "w");
    //     fseek(fp1, file_length , SEEK_SET);
    //     fputc('\n', fp1);
    // fclose(fp);
}


void worker_function_file(BoundedBuffer *request_buffer, string filename, NRC *chan2){
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    string outfilename = "received/y"+filename;


    while(true)
    {
        int f_write = open(outfilename.c_str(), O_RDWR , mode);
        
        cout<<"writing to file"<<endl;
        vector<char> msg_vec = request_buffer->pop();

        char* msg_char = new char[sizeof(filemsg) + strlen(filename.c_str()) + 1];
        msg_char = reinterpret_cast<char*>(msg_vec.data());

        if (*(MESSAGE_TYPE*) msg_char == FILE_MSG)
        {
            // char* filemsg_buf = new char[sizeof(filemsg)];
            // memcpy(filemsg_buf, msg_char, sizeof(filemsg));
            char* filemsg_buf = new char[msg_vec.size()];
            memcpy(filemsg_buf, msg_vec.data(), msg_vec.size());

            // char* filemsg_buf = msg_char + sizeof(filemsg);
            filemsg file_msg = *(filemsg*) filemsg_buf;
            // int request2 = chan2->cwrite(msg_char, sizeof(filemsg)+ strlen(filename.c_str()) + 1);
            int request2 = chan2->cwrite(filemsg_buf, sizeof(filemsg)+ strlen(filename.c_str()) + 1);
            cout<<"OFFSET IS: "<<file_msg.offset<<endl;
            char* ret_buf = (char*)chan2->cread(&request2);
            lseek(f_write, file_msg.offset, SEEK_SET);
            if (strlen(ret_buf) < MAX_MESSAGE){
                write(f_write, (void*)ret_buf, strlen(ret_buf));
            }
            else{
                write(f_write, (void*)ret_buf, MAX_MESSAGE);
            }
        }
        else{
            chan2->cwrite(msg_char,sizeof(MESSAGE_TYPE));
            delete chan2;
            break;
        }
        close(f_write);

    }

}


int main(int argc, char *argv[])
{
    int n = 15000;          //default number of requests per "patient"
    int p = 15;             // number of patients [1,15]
    int w = 1;              //default number of worker threads
    int b = 50;             // default capacity of the request buffer, you should change this default
    int m = MAX_MESSAGE;    // default capacity of the file buffer
    string name = "localhost";
    string port = "50430";
    string fname = "";

    int option;
    int fflag = 0;
    int status2;
    string filename;
    while ((option = getopt(argc,argv, "n:p:w:b:f:h:r:")) != -1){
        switch (option) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'p':
                p = atof(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'f':
                fflag = 1;
                filename = optarg;
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'h':
                name = optarg;
                break;
            case 'r':
                port = optarg;
                break;
            default:
                status2 = -1;
                break;
        }
    }

    cout<<"hostname is: "<<name<<endl;
    cout<<"port is: "<<port<<endl;
    cout<<"fflag is: "<<fflag<<endl;

    BoundedBuffer request_buffer(b);
    HistogramCollection hc;
    thread wthreads [w];


    struct timeval start, end;
    gettimeofday (&start, 0);

    
    if (fflag == 1){
        thread patients [1];

        cout<<"starting file trasnfer for file: "<<filename<<endl;
        patients[0] = thread (patient_function_file, &request_buffer, filename, name, port);
        NRC* workerchan = new NRC (name, port);

        // worker_function_file(&request_buffer, filename, workerchan);

        for (int i=0; i<w; i++){
            cout<<"making workr channel"<<endl;
            NRC* workerchan = new NRC (name, port);
            wthreads[i] = thread (worker_function_file, &request_buffer, filename, workerchan);
        }

        for(int i = 0; i < w; i++){
        MESSAGE_TYPE q = QUIT_MSG;
        char* data = (char*) &q;    
        workerchan->cwrite(data, sizeof(MESSAGE_TYPE));
        vector<char> push_msg1(data, data + sizeof(MESSAGE_TYPE));
        request_buffer.push(push_msg1);
        }
        for(int i = 0; i < w; i++){
            wthreads[i].join();
        }
    }
    else{
        thread patients [p];

        for (int i=0; i<p; i++){
            Histogram* h = new Histogram(10,2,-2);
            hc.add(h);
            patients[i] = thread (patient_function_data, (i+1), n, &request_buffer);
        }  

        for (int i=0; i<w; i++){
            NRC* workerchan = new NRC (name, port);
            wthreads[i] = thread (worker_function_data, &request_buffer, workerchan, &hc);
            // wthreads[i] = thread (worker_function_file, &request_buffer, "1.csv", workerchan);
        }
    
        for(int i = 0; i < p; i++){
            patients[i].join();
        }    
        for(int i = 0; i < w; i++){
            MESSAGE_TYPE q = QUIT_MSG;
            char* data = (char*) &q;
            vector<char> push_msg1(data, data + sizeof(MESSAGE_TYPE));
            request_buffer.push(push_msg1);
        }
        for(int i = 0; i < w; i++){
            wthreads[i].join();
        }
    }

   
    /* Join all threads here */
    gettimeofday (&end, 0);
    hc.print();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;
}