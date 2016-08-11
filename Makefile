CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -I./third_party/rapidjson/include/
CXXFLAGS += -std=c++11 -Wall -g -c -o

LIB_FILES :=-lglog -lgflags -levent -lamqp-cpp -lpthread \
	-L/usr/local/lib `pkg-config --libs grpc++ grpc` \
	-lgrpc++_reflection \
	-lprotobuf -lpthread -ldl

PROTOC = protoc
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ./protos

CPP_SOURCES := \
	./base/ascii_ctype.cc	\
	./base/file.cc	\
	./base/file_enumerator.cc \
	./base/file_path.cc	\
	./base/file_util.cc	\
	./base/location.cc	\
	./base/numbers.cc	\
	./base/once.cc	\
	./base/pickle.cc	\
	./base/ref_counted.cc	\
	./base/scoped_file.cc	\
	./base/scoped_temp_dir.cc	\
	./base/status.cc \
	./base/string_encode.cc	\
	./base/string_piece.cc	\
	./base/string_printf.cc	\
	./base/string_util.cc	\
	./base/time.cc	\
	\
	\
	\
	threading/monitor.cc	\
	threading/mutex.cc	\
	threading/thread_factory.cc	\
	threading/thread_manager.cc	\
	threading/time_util.cc	\
	\
	./server/server_interface.cc \
	./server/amqp/amqp_server.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./send \
	\
	./amqp_consumer_server \


all: $(CPP_OBJECTS) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

send: ./send.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./send.o: ./send.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./amqp_consumer_server: ./server/amqp/amqp_consumer_server.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./server/amqp/amqp_consumer_server.o: ./server/amqp/amqp_consumer_server.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

############ rpc

clean:
	rm -fr base/*.o
	rm -fr *.o
	rm -fr ./server/*.o
	rm -fr ./server/amqp/*.o
	@rm -fr $(TESTS)
	@rm -fr $(CPP_OBJECTS)
