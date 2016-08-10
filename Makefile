CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -std=c++11 -Wall -g -c -o

LIB_FILES :=-lglog -lgflags -levent -lamqp-cpp -lpthread \

CPP_SOURCES := \


CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./receive \


all: $(CPP_OBJECTS) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

receive: ./receive..o
	$(CXX) -o $@ $< $(LIB_FILES)
./receive..o: ./receive.cc
	$(CXX) $(CXXFLAGS) $@ $<

clean:
	rm -fr base/*.o
	rm -fr threading/*.o
	rm -fr zip/*.o
	rm -fr crypto/*.o
	rm -fr *.o
	@rm -fr $(TESTS)
	@rm -fr $(CPP_OBJECTS)
