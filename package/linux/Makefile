CC=${CXX:=g++}

CXXFLAGS = -std=c++11 -Iinclude -g -Wall -Wextra -O2
LDFLAGS = -lpthread -ldl

# 根据你的调试或发布构建设置相应的库目录
ifeq ($(MODE),Debug)
	LIB_DIR = Debug
else
	LIB_DIR = Release
endif

LIBS = $(LIB_DIR)/libslssdk.a \
       $(LIB_DIR)/libcurl.a \
       $(LIB_DIR)/libssl.a \
       $(LIB_DIR)/libcrypto.a \
       $(LIB_DIR)/libz.a \
       $(LIB_DIR)/libprotobuf.a

TARGET = example
SRCS = example.cpp
OBJS = $(SRCS:.cpp=.o)
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)
.PHONY: all clean