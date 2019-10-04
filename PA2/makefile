# makefile

all: dataserver client

common.o: common.h common.cpp
	g++ -g -w -std=c++11 -c common.cpp

FIFOreqchannel.o: FIFOreqchannel.h FIFOreqchannel.cpp
	g++ -g -w -std=c++11 -c FIFOreqchannel.cpp

client: client.cpp FIFOreqchannel.o common.o
	g++ -g -w -std=c++11 -o client client.cpp FIFOreqchannel.o common.o -lpthread -lrt

dataserver: dataserver.cpp  FIFOreqchannel.o common.o
	g++ -g -w -std=c++11 -o dataserver dataserver.cpp FIFOreqchannel.o common.o -lpthread -lrt

clean:
	rm -rf *.o *.csv fifo* dataserver client data*_*
