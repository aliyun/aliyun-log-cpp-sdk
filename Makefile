LIB_OUTPUT_DIR = ./lib
SLS_LIB = $(LIB_OUTPUT_DIR)/libslssdk.a
LZ4_LIB = $(LIB_OUTPUT_DIR)/liblz4.a

# use `make LZ4_INCLUDE_DIR=/path/to/lz4/include LZ4_LIB_DIR=/path/to/lz4/lib` to override lz4 lib dependency
# for example `make LZ4_INCLUDE_DIR=/usr/local/include LZ4_LIB_DIR=/usr/local/lib`
# if LZ4_INCLUDE_DIR and LZ4_LIB_DIR are not set, build lz4 from source
LZ4_INCLUDE_DIR ?= ./lz4
LZ4_LIB_DIR ?= 
LZ4_SRC_DIR = ./lz4

# flags
COMMON_FLAGS = -g -Wall -Wextra -pedantic
CFLAGS += $(COMMON_FLAGS) -O3 -std=c99 -Wundef -Wshadow -Wcast-align -Wstrict-prototypes
CXXFLAGS += $(COMMON_FLAGS) -O2 -std=c++11 -Wno-unused-parameter -Wno-ignored-qualifiers -I$(LZ4_INCLUDE_DIR)
LDFLAGS += -L$(LIB_OUTPUT_DIR) -lslssdk -llz4 -lcurl -lpthread 

# source files
SRCS = adapter.cpp common.cpp resource.cpp client.cpp pb.cpp coding.cpp
OBJS = $(SRCS:.cpp=.o)

LZ4_SRCS = $(LZ4_SRC_DIR)/lz4.c $(LZ4_SRC_DIR)/xxhash.c
LZ4_OBJS = lz4.o xxhash.o

# targets

SLS_LIB_DEPS = $(OBJS)
# if LZ4_LIB_DIR is not set, build lz4 from source
ifeq ($(LZ4_LIB_DIR), )
    SLS_LIB_DEPS += $(LZ4_LIB)
else
	LDFLAGS += -L$(LZ4_LIB_DIR)
endif

# sls lib
$(SLS_LIB): $(SLS_LIB_DEPS)
	@echo "Building static library $(SLS_LIB_DEPS)"
	mkdir -p $(LIB_OUTPUT_DIR)
	ar rcs $@ $(OBJS)

# sample
SAMPLE = sample
$(SAMPLE): $(OBJS) $(SLS_LIB) sample.o
	$(CXX) -o $@ sample.o $(CXXFLAGS) $(LDFLAGS)

sample.o: sample.cpp
	$(CXX) -c $< $(CXXFLAGS)

# lz4
$(LZ4_LIB): $(LZ4_OBJS)
	ar rcs $@ $(LZ4_OBJS)

# Pattern rules for object files
%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

%.o: lz4/%.c
	$(CC) -c $< $(CFLAGS)

# Clean up
clean:
	rm -f $(OBJS) $(SAMPLE) $(SLS_LIB)
	rm -f *.o
	rm -f ./lib/*.a
