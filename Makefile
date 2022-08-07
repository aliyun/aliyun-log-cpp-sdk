CFLAGS= -O2 -g  -std=gnu++11
LIBDIR=-L./lib/
LIBS=-lslssdk
LIBS+=-lsls_logs_pb
LIBS+=-llz4
LIBS+=-lcurl
LIBS+=-lprotobuf
LIBS+= -lpthread

main:  sample.o libslssdk.a libsls_logs_pb.a
	g++ -o sample sample.o  $(CFLAGS) $(LIBDIR) $(LIBS)

libslssdk.a:client.o resource.o adapter.o common.o signer.o
	ar rc lib/libslssdk.a  client.o resource.o adapter.o common.o signer.o

libsls_logs_pb.a : sls_logs.pb.cc
	g++ -c  sls_logs.pb.cc $(CFLAGS)
	ar rc lib/libsls_logs_pb.a sls_logs.pb.o

proto.cc: sls_logs.proto
	protoc  --cpp_out=./ sls_logs.proto

adapter.o:   adapter.cpp
	g++ -c -g adapter.cpp $(CFLAGS)

common.o: common.cpp
	g++ -c -g common.cpp $(CFLAGS)

resource.o:  resource.cpp
	g++ -c  resource.cpp $(CFLAGS)

client.o: client.cpp  proto.cc
	g++ -c  client.cpp $(CFLAGS)

signer.o: signer.cpp
	g++ -c signer.cpp $(CFLAGS)

sample.o:sample.cpp proto.cc
	g++ -c  sample.cpp $(CFLAGS)



clean:
	rm -f adapter.o common.o resource.o client.o sample.o sample signer.o sls_logs.pb.o lib/libslssdk.a sls_logs.pb.h sls_logs.pb.cc lib/libsls_logs_pb.a
