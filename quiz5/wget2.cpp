#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
using namespace std;

int socket_connect(char *host, in_port_t port){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1, sock;     

	if((hp = gethostbyname(host)) == NULL){
		herror("gethostbyname");
		exit(1);
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	if(sock == -1){
		perror("setsockopt");
		exit(1);
	}
	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect");
		exit(1);

	}
	return sock;
}
 
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	int fd;
	char buffer[BUFFER_SIZE];
	if(argc < 2){
		fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
		exit(1); 
	}
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    string fullpath = argv[1];
    string removehttp = fullpath;
    std::size_t found = fullpath.find("https://");
    if (found!=std::string::npos){
        cout<<"in found for https"<<endl;
        removehttp = fullpath.substr(found +8);
        cout<<removehttp<<endl;
    }
    string okok = "\r\n\r\n";
	cout<<"SIZE OF rnrn is: "<<okok.size()<<endl;

    found = fullpath.find("http://");
    if (found!=std::string::npos){
        cout<<"in found for http"<<endl;
        removehttp = fullpath.substr(found +7);
        cout<<removehttp<<endl;
    }
    string pathname = "";
	string removewww = removehttp;

	std::size_t found = fullpath.find("www.");
    if (found!=std::string::npos){
        cout<<"in found for www."<<endl;
        removewww = removehttp.substr(found +8);
        cout<<removehttp<<endl;
    }
    string hostname = removewww;

    found = removewww.find(".com");
    if (found!=std::string::npos){
		hostname.resize(found +4);
		if (strlen(removewww.c_str()) != (found +4)){
			cout<<"in found for .com"<<endl;
			pathname = removewww.substr(found +4) + " ";
			cout<<hostname<<endl;
		}

    }
    found = removewww.find(".edu");
    if (found!=std::string::npos){
		hostname.resize(found +4);
		if (strlen(removewww.c_str()) != (found +4)){		
			cout<<"in found for edu"<<endl;
			pathname = removewww.substr(found +4) + " ";
			hostname.resize(found +4);
			cout<<pathname<<endl;
		}
    }



    // cout<<"ok";
    char http[(hostname.size() + 1)];

    strcpy(http, hostname.c_str()); 
    // cout<<http<<endl;
	fd = socket_connect(http, 80);

	string request;

	if (pathname == ""){
		request = "GET /\r\n";
	}
    else{
		// request =  "GET " + pathname + "HTTP/1.1\r\nHost: " + hostname + "\r\n\r\n";
		request =  "GET " + pathname + "HTTP/1.1\r\nhost: " + hostname + "\r\nConnection: close\r\n\r\n";
	}
	cout<<"final request is: "<<request<<endl;
    // string request = "GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1";
	write(fd, request.c_str(), strlen(request.c_str()));
	bzero(buffer, BUFFER_SIZE);
	int fd1 = open("download.html", O_CREAT | O_RDWR, mode);
	while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
		string temp = buffer;
		string test = temp;
		std::size_t found1 = temp.find("\r\n\r\n");
    	if (found1!=std::string::npos){
			test = temp.substr(found1 + 4);
		}
		if (strlen(test.c_str()) != strlen(temp.c_str())){
			bzero(buffer, test.size()+1);
			strcpy(buffer, test.c_str());
			write(fd1, buffer, strlen(buffer));
			fprintf(stderr, "%s", buffer);
			bzero(buffer, BUFFER_SIZE);
		}
		else{
        	write(fd1, buffer, strlen(buffer));
			fprintf(stderr, "%s", buffer);
			bzero(buffer, BUFFER_SIZE);
		}
	}

	close(fd1);
	shutdown(fd, SHUT_RDWR); 
	close(fd); 
    

	return 0;
}


