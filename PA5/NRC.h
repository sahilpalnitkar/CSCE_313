#ifndef NRC_H
#define NRC_H

#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>

class NRC{
    private:
        int sockfd;
        string pipe1, pipe2;
        int open_pipe(string _pipe_name, int mode);

    public:

        NRC(const string host_name, const string port /*16 bit short*/); //client side
        NRC(string port, void (*handle_process_loop)(NRC*)); //server side
        NRC(int fd);
        ~NRC();
        int cwrite(char *msg, int msglen);
        char* cread(int *len=NULL);
};

#endif