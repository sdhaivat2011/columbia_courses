OBJS = index.o api/ioformat.o parsers/xml/pugixml/src/pugixml.o db/sqlite3/sqlite3.o backend/stemming/porter/stem.o
OBJS1 = query.o api/ioformat.o parsers/xml/pugixml/src/pugixml.o db/sqlite3/sqlite3.o backend/stemming/porter/stem.o
CC = g++
GCC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all : index query 

index : $(OBJS)
	    $(CC) $(LFLAGS) $(OBJS) -o index -lpthread -ldl

query : $(OBJS1)
	    $(CC) $(LFLAGS) $(OBJS1) -o query -lpthread -ldl

index.o : index.cpp api/ioformat.h backend/stemming/porter/stem.h
	    $(CC) $(CFLAGS) index.cpp

api/ioformat.o : api/ioformat.h api/ioformat.cpp parsers/xml/pugixml/src/pugixml.hpp db/sqlite3/sqlite3.h
	    $(CC) $(CFLAGS) api/ioformat.cpp -o api/ioformat.o

parsers/xml/pugixml/src/pugixml.o : parsers/xml/pugixml/src/pugixml.hpp
	    $(CC) $(CFLAGS) parsers/xml/pugixml/src/pugixml.cpp -o parsers/xml/pugixml/src/pugixml.o

db/sqlite3/sqlite3.o : db/sqlite3/sqlite3.h db/sqlite3/sqlite3ext.h
	    $(GCC) $(CFLAGS) db/sqlite3/sqlite3.c -o db/sqlite3/sqlite3.o

backend/stemming/porter/stem.o : backend/stemming/porter/stem.h
	    $(GCC) $(CFLAGS) backend/stemming/porter/stem.c -o backend/stemming/porter/stem.o

query.o : query.cpp api/ioformat.h backend/stemming/porter/stem.h
	    $(CC) $(CFLAGS) query.cpp

clean:
	    \rm *.o api/*.o parsers/xml/pugixml/src/*.o db/sqlite3/*.o backend/stemming/porter/stem.o ../indexes/*.sqlite docInfo.dat invertedIndex.dat index query
