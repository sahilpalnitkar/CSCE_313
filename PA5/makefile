# makefile

all: dataserver client

common.o: common.h common.cpp
	g++ -g -w -std=c++11 -c common.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -std=c++11 -c Histogram.cpp

NRC.o: NRC.h NRC.cpp
	g++ -g -w -std=c++11 -c NRC.cpp

client: client.cpp Histogram.o NRC.o common.o
	g++ -g -w -std=c++11 -o client client.cpp Histogram.o NRC.o common.o -lpthread -lrt

dataserver: dataserver.cpp  NRC.o common.o
	g++ -g -w -std=c++11 -o dataserver dataserver.cpp NRC.o common.o -lpthread -lrt

clean:
	rm -rf *.o fifo* dataserver client 
