OBJS = index.o api/input.o parsers/xml/pugixml/src/pugixml.o db/sqlite3/sqlite3.o backend/stemming/porter/stem.o
CC = g++
GCC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

index : $(OBJS)
	    $(CC) $(LFLAGS) $(OBJS) -o index

index.o : index.cpp api/input.h backend/stemming/porter/stem.h
	    $(CC) $(CFLAGS) index.cpp

api/input.o : api/input.h api/input.cpp parsers/xml/pugixml/src/pugixml.hpp db/sqlite3/sqlite3.h
	    $(CC) $(CFLAGS) api/input.cpp -o api/input.o

parsers/xml/pugixml/src/pugixml.o : parsers/xml/pugixml/src/pugixml.hpp
	    $(CC) $(CFLAGS) parsers/xml/pugixml/src/pugixml.cpp -o parsers/xml/pugixml/src/pugixml.o

db/sqlite3/sqlite3.o : db/sqlite3/sqlite3.h db/sqlite3/sqlite3ext.h
	    $(GCC) $(CFLAGS) db/sqlite3/sqlite3.c -o db/sqlite3/sqlite3.o

backend/stemming/porter/stem.o : backend/stemming/porter/stem.h
	    $(GCC) $(CFLAGS) backend/stemming/porter/stem.c -o backend/stemming/porter/stem.o

clean:
	    \rm *.o api/*.o parsers/xml/pugixml/src/*.o db/sqlite3/*.o backend/stemming/porter/stem.o ../indexes/*.sqlite docInfo.dat index
