/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/19
 */
#include "common.h"
#include "FIFOreqchannel.h"

using namespace std;


int main(int argc, char *argv[]){
    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));

    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    // sending a non-sense message, you need to change this
    char x = 55;
    chan.cwrite (&x, sizeof (x));
    char* buf = chan.cread ();


    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));

   
}
