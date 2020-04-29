CFLAGS= -O2 -L./lib/
#LIBS=-lslssdk
LIBS=-lsls_logs_pb
LIBS+=-llz4
LIBS+=-lcurl
LIBS+=-lprotobuf
LIBS+=-lpthread


main:  sample.o libslssdk.a libsls_logs_pb.a
	g++ -o sample sample.o $(CFLAGS) -lslssdk $(LIBS)

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

# manually use 'make wrapper' to make these rules, only support for c++11+
wrapper:  sample_wrapper.o libslssdk_wrapper.a libsls_logs_pb.a
	g++ -o sample_wrapper sample_wrapper.o $(CFLAGS) -lslssdk_wrapper $(LIBS)

libslssdk_wrapper.a:client.o resource.o adapter.o common.o sls_client.o
	ar rc lib/libslssdk_wrapper.a  client.o resource.o adapter.o common.o sls_client.o

sls_client.o:client_wrapper/sls_client.cpp
	g++ --std=c++11 -c client_wrapper/sls_client.cpp

sample_wrapper.o:sample_wrapper.cpp proto.cc
	g++ --std=c++11 -c sample_wrapper.cpp


clean:
	rm -f adapter.o common.o resource.o client.o sample.o sample sls_logs.pb.o lib/libslssdk.a sls_logs.pb.h sls_logs.pb.cc lib/libsls_logs_pb.a
	rm -f sample_wrapper sample_wrapper.o sls_client.o lib/libslssdk_wrapper.a
