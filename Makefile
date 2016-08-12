CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -I./third_party/rapidjson/include/
CXXFLAGS += -std=c++11 -Wall -g -c -o

LIB_FILES :=-lglog -lgflags -levent -lamqp-cpp -lpthread \
	-L/usr/local/lib -lgrpc++ -lgrpc \
	-lgrpc++_reflection \
	-lprotobuf -lpthread -ldl \
	-lmysqlclient

EPUB_INFO_LIBS=./third_party/epubtools/libepubtools.a -lz -lmxml -lgumbo -lssl -lpthread

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
	\
	./db/common/connection_info.cc \
	./db/common/connection_pool.cc \
	./db/common/connection_manager.cc \
	./db/backend/db_result.cc \
	./db/backend/db_statement.cc \
	./db/backend/db_connection.cc \
	./db/backend/connector_interface.cc \
	\
	./db/drivers/mysql/mysql_connector.cc \
	./db/drivers/mysql/mysql_direct_result.cc \
	./db/drivers/mysql/mysql_direct_statement.cc \
	./db/drivers/mysql/mysql_prepared_result.cc \
	./db/drivers/mysql/mysql_prepared_statement.cc \
	./db/drivers/mysql/mysql_connection.cc \
	\
	./db/frontend/result.cc \
	./db/frontend/statement.cc \
	./db/frontend/session.cc \
	./db/frontend/transaction.cc \
	\
	\
	./server/server_interface.cc \
	./server/amqp/amqp_server.cc \
	\
	\
	./protos/epub_info.pb.cc \
	./protos/epub_info.grpc.pb.cc \
	\
	./service/rpc_epub_info_handler.cc \


CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./send \
	\
	./amqp_consumer_server \
	\
	./epub_info_service \


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

./epub_info_service: ./rpc/epub_info_service/epub_info_service.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(EPUB_INFO_LIBS)
./rpc/epub_info_service/epub_info_service.o: ./rpc/epub_info_service/epub_info_service.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


vpath %.proto $(PROTOS_PATH)

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	@echo "  [GEN]  $@"
	@$(PROTOC) -I $(PROTOS_PATH) --grpc_out=$(PROTOS_PATH) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	@echo "  [GEN]  $@"
	@$(PROTOC) -I $(PROTOS_PATH) --cpp_out=$(PROTOS_PATH) $<

############ rpc

clean:
	rm -fr base/*.o
	rm -fr *.o
	rm -fr ./server/*.o
	rm -fr ./server/amqp/*.o
	rm -fr ./protos/*.o 
	rm -fr ./protos/*.pb.cc 
	rm -fr ./protos/*.pb.h
	rm -fr ./rpc/epub_info_service/*.o
	@rm -fr $(TESTS)
	@rm -fr $(CPP_OBJECTS)
