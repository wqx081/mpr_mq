CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -std=c++11 -Wall -g -c -o

LIB_FILES :=-lglog -lgflags -levent -lamqp-cpp -lpthread \

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
	./server/server_interface.cc \
	./server/amqp/amqp_server.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./receive \


all: $(CPP_OBJECTS) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

receive: ./receive..o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./receive..o: ./receive.cc
	$(CXX) $(CXXFLAGS) $@ $<

clean:
	rm -fr base/*.o
	rm -fr *.o
	@rm -fr $(TESTS)
	@rm -fr $(CPP_OBJECTS)
