# for gcc on linux
CFLAGS= -O2  -std=gnu++11 -fPIC -Wall -pedantic
LINK_FLAGS=-L./lib/
LIBS=-lsls-sdk-cpp-static
LIBS+=-lsls_logs_pb
LIBS+=-llz4
LIBS+=-lcurl
LIBS+=-lprotobuf
LIBS+= -lpthread
INCLUDES=-Iinclude
INCLUDES+=-I./

main:  sample.o libsls-sdk-cpp-static.a libsls_logs_pb.a liblz4.a
	g++ -o sample sample.o  $(CFLAGS) $(LIBS) $(INCLUDES) $(LINK_FLAGS)

libsls-sdk-cpp-static.a:client.o resource.o adapter.o common.o 
	ar rc lib/libsls-sdk-cpp-static.a  client.o resource.o adapter.o common.o 

libsls_logs_pb.a : sls_logs.pb.cc
	g++ -c  sls_logs.pb.cc
	ar rc lib/libsls_logs_pb.a sls_logs.pb.o

liblz4.a:
	 $(MAKE) -C include/lz4 lib
	 cp include/lz4/liblz4.a lib/

sls_logs.pb.cc: sls_logs.proto
	protoc  --cpp_out=./ sls_logs.proto

adapter.o:   adapter.cpp
	g++ -c  adapter.cpp $(CFLAGS) $(INCLUDES)

common.o: common.cpp
	g++ -c  common.cpp $(CFLAGS) $(INCLUDES)

resource.o:  resource.cpp
	g++ -c  resource.cpp $(CFLAGS)  $(INCLUDES)

client.o: client.cpp  sls_logs.pb.cc
	g++ -c  client.cpp $(CFLAGS) $(INCLUDES)

sample.o: example/sample.cpp sls_logs.pb.cc
	g++ -c  example/sample.cpp -o sample.o $(CFLAGS) $(INCLUDES)



clean:
	rm -f adapter.o common.o resource.o client.o sample.o sample sls_logs.pb.o lib/libsls-sdk-cpp-static.a sls_logs.pb.h sls_logs.pb.cc lib/libsls_logs_pb.a lib/liblz4.a
	$(MAKE) -C include/lz4 $@
