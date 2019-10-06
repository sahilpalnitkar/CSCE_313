/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/19
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;

// double get_data_from_server(FIFORequestChannel chan, int patient_number, double time_of_ecg, int ecg_record){
//     datamsg data = datamsg (patient_number, time_of_ecg, ecg_record);
//     chan.cwrite(&data, sizeof(data));
//     char* buf = chan.cread();

//     double ecg_value = *(double*)buf;
//     return ecg_value;
// }

// void get_file_data_points_from_server(FIFORequestChannel chan, int patient_number){
//     cout<<"Started data pointsfunction"<<endl;
//     stringstream ss;
//     ss << "x" << patient_number << ".csv";
//     string filename = ss.str();
//     cout<<filename<<endl;

    
//     ofstream outputFile;  
//     outputFile.open(filename, ios::out | ios::app | ios::binary);
//     for (double x = 0; x < 59.996; x = x +0.004){
//         datamsg data1 = datamsg (patient_number, x, 1);
//         chan.cwrite(&data1, sizeof(data1));
//         char* buf1 = chan.cread();
//         double ecg_value1 = *(double*)buf1;

//         datamsg data2 = datamsg (patient_number, x, 2);
//         chan.cwrite(&data2, sizeof(data2));
//         char* buf2 = chan.cread();
//         double ecg_value2 = *(double*)buf2;
//         cout<<x<<", "<<ecg_value1<<", "<<ecg_value2<<endl;
//         outputFile<<x<<","<<ecg_value1<<","<<ecg_value2<<endl;
//     }
//     outputFile.close();

//     filemsg file_message1 = filemsg(0,0);
//     chan.cwrite(&file_message1,sizeof(file_message1));
// }

// void get_file_chunks_from_server(FIFORequestChannel chan, int patient_number){
//     ofstream outputFile;


//     stringstream ss;
//     ss << patient_number << ".csv";
//     string filename = ss.str();
//     string outfilename = "y"+filename;
//     outputFile.open(outfilename, ios::out | ios::app | ios::binary);
//     cout<<filename<<endl;
//     filemsg file = filemsg(0,0);
//     char* buf = new char[sizeof(filemsg) + filename.length() + 1];
//     memcpy(buf, &file, sizeof(filemsg));
//     strcpy(buf + sizeof(filemsg), filename.c_str());
//     chan.cwrite(buf, sizeof(filemsg)+filename.length()+1);
//     int file_length = *(int*)chan.cread();
//     cout<<"FILE LENGTH IS: "<<file_length<<endl;
//     int file_request_counter = file_length/MAX_MESSAGE;
//     int remaining_file_length = MAX_MESSAGE*file_request_counter;
//     cout<<"FILE REQUEST COUNTER IS: "<<file_request_counter<<endl;
//     while(file_length > MAX_MESSAGE){
//         for (int i = 0; i<file_request_counter; i++){
//             if (i == 0){
//                 filemsg request_1 = filemsg(0,MAX_MESSAGE);
//                 char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
//                 memcpy(buf2, &request_1, sizeof(filemsg));
//                 strcpy(buf2 + sizeof(filemsg), filename.c_str());                
//                 chan.cwrite(buf2, sizeof(filemsg)+ filename.length() + 1);
//                 char* ret_buf = chan.cread();
//                 cout<<ret_buf;
//                 file_length = file_length - MAX_MESSAGE;
//                 outputFile << ret_buf;
//             }
//             else{
//                 filemsg request_1 = filemsg(((MAX_MESSAGE*i)),(MAX_MESSAGE));
//                 char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
//                 memcpy(buf2, &request_1, sizeof(filemsg));
//                 strcpy(buf2 + sizeof(filemsg), filename.c_str());
//                 chan.cwrite(buf2, sizeof(filemsg)+ filename.length() + 1);
//                 char* ret_buf = chan.cread();
//                 cout<<ret_buf;
//                 file_length = file_length - MAX_MESSAGE;
//                 outputFile << ret_buf;
//             }
//         }
//     }
//     filemsg request_1 = filemsg(remaining_file_length, file_length);
//     char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
//     memcpy(buf2, &request_1, sizeof(filemsg));
//     strcpy(buf2 + sizeof(filemsg), filename.c_str());
//     chan.cwrite(buf2, sizeof(filemsg)+ filename.length() + 1);
//     char* ret_buf = chan.cread();
//     cout<<ret_buf;
//     outputFile << ret_buf;
//     outputFile.close();

    


// }

int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    bool pflag = 0;
    bool tflag = 0;
    bool eflag = 0;
    bool fflag = 0;
    bool cflag = 0;
    srand(time_t(NULL));
    int status;
    int patient_number;
    double time_of_ecg;
    int ecg_record;
    int option;
    string filename = "";
    // cout<<"pflag: "<<pflag<<endl;
    // cout<<"tflag: "<<tflag<<endl;
    // cout<<"eflag: "<<eflag<<endl;
    // cout<<"fflag: "<<fflag<<endl;
    // cout<<"cflag: "<<cflag<<endl;

    while ((option = getopt(argc,argv, "p:t:e:f:c")) != -1){
        switch (option) {
            case 'p':
                pflag = 1;
                patient_number = atoi(optarg);
                break;
                // cout<<"PATIENT NUMBER IS: "<<patient_number<<endl;
            case 't':
                tflag = 1;
                time_of_ecg = atoi(optarg);
                break;
            
                // cout<<"TIME IS: "<<time_of_ecg<<endl;
            case 'e':
                eflag = 1;
                ecg_record = atoi(optarg);
                break;
                // cout<<"ECG RECORD IS: "<<ecg_record<<endl; 
            case 'f':
                fflag = 1;
                filename = optarg;
                break;
            case 'c':
                cflag = 1;
                break;
                // cout<<"null"<<endl;
            

            default:
            status = -1;
                // cout<<"ERROR"<<endl;
                // break;
        }
    }
    // cout<<"pflag: "<<pflag<<endl;
    // cout<<"tflag: "<<tflag<<endl;
    // cout<<"eflag: "<<eflag<<endl;
    // cout<<"fflag: "<<fflag<<endl;
    // cout<<"cflag: "<<cflag<<endl;

    if ((pflag)&&(tflag)&&(eflag)){
        FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
        datamsg data = datamsg (patient_number, time_of_ecg, ecg_record);
        chan.cwrite(&data, sizeof(data));
        char* buf = chan.cread();

        double ecg_value = *(double*)buf;
        cout<<"ECG VALUE: "<<ecg_value<<endl;
        MESSAGE_TYPE m = QUIT_MSG;
        chan.cwrite(&m, sizeof(MESSAGE_TYPE));
        return 0;
    }
    else if((pflag) && (!tflag)&&(!eflag)){
        FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
        cout<<"Started data points function"<<endl;
        stringstream ss;
        ss << "x" << patient_number << ".csv";
        string outfilename = ss.str();
        cout<<outfilename<<endl;

        
        ofstream outputFile;  
        outputFile.open(outfilename, ios::out | ios::app | ios::binary);
        for (double x = 0; x < 59.996; x = x +0.004){
            datamsg data1 = datamsg (patient_number, x, 1);
            chan.cwrite(&data1, sizeof(data1));
            char* buf1 = chan.cread();
            double ecg_value1 = *(double*)buf1;

            datamsg data2 = datamsg (patient_number, x, 2);
            chan.cwrite(&data2, sizeof(data2));
            char* buf2 = chan.cread();
            double ecg_value2 = *(double*)buf2;
            cout<<x<<", "<<ecg_value1<<", "<<ecg_value2<<endl;
            outputFile<<x<<","<<ecg_value1<<","<<ecg_value2<<endl;
        }
        outputFile.close();
        MESSAGE_TYPE m = QUIT_MSG;
        chan.cwrite(&m, sizeof(MESSAGE_TYPE));
    }
    else if (fflag){
        FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
        ofstream outputFile;
        string outfilename = "y"+filename;
        outputFile.open(outfilename, ios::out | ios::app | ios::binary);
        cout<<filename<<endl;
        filemsg file = filemsg(0,0);
        char* buf = new char[sizeof(filemsg) + filename.length() + 1];
        memcpy(buf, &file, sizeof(filemsg));
        strcpy(buf + sizeof(filemsg), filename.c_str());
        chan.cwrite(buf, sizeof(filemsg)+filename.length()+1);
        int file_length = *(int*)chan.cread();
        cout<<"FILE LENGTH IS: "<<file_length<<endl;
        int file_request_counter = file_length/MAX_MESSAGE;
        int remaining_file_length = MAX_MESSAGE*file_request_counter;
        cout<<"FILE REQUEST COUNTER IS: "<<file_request_counter<<endl;
        while(file_length > MAX_MESSAGE){
            for (int i = 0; i<file_request_counter; i++){
                if (i == 0){
                    filemsg request_1 = filemsg(0,MAX_MESSAGE);
                    char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
                    memcpy(buf2, &request_1, sizeof(filemsg));
                    strcpy(buf2 + sizeof(filemsg), filename.c_str());                
                    chan.cwrite(buf2, sizeof(filemsg)+ filename.length() + 1);
                    char* ret_buf = chan.cread();
                    // cout<<ret_buf;
                    file_length = file_length - MAX_MESSAGE;
                    outputFile << ret_buf;
                }
                else{
                    filemsg request_1 = filemsg(((MAX_MESSAGE*i)),(MAX_MESSAGE));
                    char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
                    memcpy(buf2, &request_1, sizeof(filemsg));
                    strcpy(buf2 + sizeof(filemsg), filename.c_str());
                    chan.cwrite(buf2, sizeof(filemsg)+ filename.length() + 1);
                    char* ret_buf = chan.cread();
                    // cout<<ret_buf;
                    file_length = file_length - MAX_MESSAGE;
                    outputFile << ret_buf;
                }
            }
        }
        filemsg request_1 = filemsg(remaining_file_length, file_length);
        char* buf2 = new char[sizeof(filemsg)+ filename.length() + 1];
        memcpy(buf2, &request_1, sizeof(filemsg));
        strcpy(buf2 + sizeof(filemsg), filename.c_str());
        chan.cwrite(buf2, sizeof(filemsg)+ filename.length() + 1);
        char* ret_buf = chan.cread();
        // cout<<ret_buf;
        outputFile << ret_buf;
        outputFile.close();
        MESSAGE_TYPE m = QUIT_MSG;
        chan.cwrite(&m, sizeof(MESSAGE_TYPE));
        return 0;
    }
    else if(cflag){
        cout<<"MAKING NEW SERVER"<<endl;
        FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
        // newchannelmsg newchannel = newchannelmsg();
        MESSAGE_TYPE newchannel = NEWCHANNEL_MSG;
        chan.cwrite(&newchannel, sizeof(MESSAGE_TYPE));
        string channel_name = chan.cread();
        cout<<channel_name<<endl;
        FIFORequestChannel chan2 (channel_name, FIFORequestChannel::CLIENT_SIDE);
        datamsg data1 = datamsg (1,1,1);
        chan2.cwrite(&data1, sizeof(datamsg));
        double new_channel_data = *(double*)chan2.cread();
        cout<<new_channel_data<<endl;
        // get_file_chunks_from_server(chan, 1);
        // get_file_data_points_from_server(chan2, 2);
            // closing the channel    
        MESSAGE_TYPE m = QUIT_MSG;
        chan2.cwrite(&m, sizeof(MESSAGE_TYPE));
        chan.cwrite(&m, sizeof(MESSAGE_TYPE));
    }
    else{cout<<"ERROR"<<endl;}
}
