CFLAGS= -O2 -L./lib/ -std=gnu++11
LIBS=-lslssdk
LIBS+=-lsls_logs_pb
LIBS+=-llz4
LIBS+=-lcurl
LIBS+=-lprotobuf
LIBS+= -lpthread


main:  sample.o libslssdk.a libsls_logs_pb.a
	g++ -o sample sample.o   $(CFLAGS) $(LIBS)

libslssdk.a:client.o resource.o adapter.o common.o 
	ar rc lib/libslssdk.a  client.o resource.o adapter.o common.o 

libsls_logs_pb.a : sls_logs.pb.cc
	g++ -c  sls_logs.pb.cc
	ar rc lib/libsls_logs_pb.a sls_logs.pb.o

proto.cc: sls_logs.proto
	protoc  --cpp_out=./ sls_logs.proto

adapter.o:   adapter.cpp
	g++ -c  adapter.cpp

common.o: common.cpp
	g++ -c  common.cpp

resource.o:  resource.cpp
	g++ -c  resource.cpp

client.o: client.cpp  proto.cc
	g++ -c  client.cpp

sample.o:sample.cpp proto.cc
	g++ -c  sample.cpp



clean:
	rm -f adapter.o common.o resource.o client.o sample.o sample sls_logs.pb.o lib/libslssdk.a sls_logs.pb.h sls_logs.pb.cc lib/libsls_logs_pb.a
