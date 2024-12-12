# Compiler and flags
CXXFLAGS ?= -O2 -std=gnu++11 -g
LDFLAGS ?= -L./lib/
LIBS = -lslssdk -llz4 -lcurl -lpthread

MOREFLAGS ?= -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-ignored-qualifiers
CXXFLAGS += $(MOREFLAGS)

# Source files and directories
SRC_DIR = .
LIB_DIR = ./lib

# Targets
TARGET = sample
LIB1 = $(LIB_DIR)/libslssdk.a

# Source files
SRCS = adapter.cpp common.cpp resource.cpp client.cpp sample.cpp pb.cpp coding.cpp
OBJS = $(SRCS:.cpp=.o)


# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS) $(LIB1)
	$(CXX) -o $@ sample.o $(CXXFLAGS) $(LDFLAGS) $(LIBS)

# Static libraries
$(LIB1): client.o resource.o adapter.o common.o pb.o coding.o
	ar rc $@ $^

# Pattern rules for object files
%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

# Clean up
clean:
	rm -f $(OBJS) $(TARGET) $(LIB1)
