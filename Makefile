# for gcc on linux
CFLAGS= -O2 -L./lib/ -std=gnu++11
LIBS=-lslssdk
LIBS+=-lsls_logs_pb
LIBS+=-llz4
LIBS+=-lcurl
LIBS+=-lprotobuf
LIBS+= -lpthread
INCLUDES=-Iinclude

main:  sample.o libslssdk.a libsls_logs_pb.a
	g++ -o sample sample.o   $(CFLAGS) $(LIBS) $(INCLUDES)

libslssdk.a:client.o resource.o adapter.o common.o 
	ar rc lib/libslssdk.a  client.o resource.o adapter.o common.o 

libsls_logs_pb.a : sls_logs.pb.cc
	g++ -c  sls_logs.pb.cc
	ar rc lib/libsls_logs_pb.a sls_logs.pb.o

proto.cc: sls_logs.proto
	protoc  --cpp_out=./ sls_logs.proto

adapter.o:   adapter.cpp
	g++ -c  adapter.cpp $(CFLAGS) $(LIBS) $(INCLUDES)

common.o: common.cpp
	g++ -c  common.cpp $(CFLAGS) $(LIBS) $(INCLUDES)

resource.o:  resource.cpp
	g++ -c  resource.cpp $(CFLAGS) $(LIBS) $(INCLUDES)

client.o: client.cpp  sls_logs.pb.cc
	g++ -c  client.cpp $(CFLAGS) $(LIBS) $(INCLUDES)

sample.o:sample.cpp sls_logs.pb.cc
	g++ -c  sample.cpp $(CFLAGS) $(LIBS) $(INCLUDES)



clean:
	rm -f adapter.o common.o resource.o client.o sample.o sample sls_logs.pb.o lib/libslssdk.a sls_logs.pb.h sls_logs.pb.cc lib/libsls_logs_pb.a
