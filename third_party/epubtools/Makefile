APP=libepubtools.a

CXXFLAGS += -I.
CXXFLAGS += -I./inc
CXXFLAGS += -I./public/info
CXXFLAGS += -I./public/splitter
CXXFLAGS += -I./src/epub
CXXFLAGS += -I./src/splitter
CXXFLAGS += -I./src/tools
CXXFLAGS += -I./src/zipio/
CXXFLAGS += -I./src/info
CXXFLAGS += -std=c++11 -g -c -o

CFLAGS += -I./zipio/internal
CFLAGS += -g -c -o

all: $(APP)

CPP_SOURCES := ./src/epub/EpubPackage.cpp \
	./src/epub/EpubPackageWriter.cpp \
	./src/epub/HtmlResourceFetcher.cpp \
	./src/epub/TEpubArchiveSource.cpp \
	./src/info/EpubInfo.cpp \
	./src/splitter/EpubSplitter.cpp \
	./src/tools/TCharactorUtils.cpp \
	./src/tools/TDirUtil.cpp \
	./src/tools/TGumboUtils.cpp \
	./src/tools/TPathUtil.cpp \
	./src/tools/TStringUtils.cpp \
	./src/tools/TUtf8Util.cpp \
	./src/tools/TXMLReader.cpp \
	./src/tools/TXMLWriter.cpp \
	./src/zipio/BufferStream.cpp \
	./src/zipio/CombinedStream.cpp \
	./src/zipio/DecompressStream.cpp \
	./src/zipio/FileStream.cpp \
	./src/zipio/IOAdapter.cpp \
	./src/zipio/IStreamBase.cpp \
	./src/zipio/OffsetStream.cpp \
	./src/zipio/ZipArchiveReader.cpp \
	./src/zipio/ZipArchiveWriter.cpp \
	

C_SOURCES := ./src/zipio/internal/ioapi.c \
	./src/zipio/internal/mztools.c \
	./src/zipio/internal/unzip.c \
	./src/zipio/internal/zip.c \

CPP_OBJECTS := $(CPP_SOURCES:.cpp=.o)
C_OBJECTS := $(C_SOURCES:.c=.o)

.cpp.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

.c.o:
	@echo "  [CC]   $@"
	@$(CC) $(CFLAGS) $@ $<

OBJECTS := $(CPP_OBJECTS)
OBJECTS += $(C_OBJECTS)

$(APP): $(OBJECTS)
	@echo "  [AR]   $@"
	@ar rcs $(APP) $(OBJECTS)

clean:
	@rm -f $(CPP_OBJECTS)
	@rm -f $(C_OBJECTS)
	@rm -f $(APP)
