OBJS = index.o api/input.o parsers/xml/pugixml/src/pugixml.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

index : $(OBJS)
	    $(CC) $(LFLAGS) $(OBJS) -o index

index.o : index.cpp api/input.h
	    $(CC) $(CFLAGS) index.cpp

api/input.o : api/input.h api/input.cpp parsers/xml/pugixml/src/pugixml.hpp
	    $(CC) $(CFLAGS) api/input.cpp -o api/input.o

parsers/xml/pugixml/src/pugixml.o : parsers/xml/pugixml/src/pugixml.hpp
	    $(CC) $(CFLAGS) parsers/xml/pugixml/src/pugixml.cpp -o parsers/xml/pugixml/src/pugixml.o

clean:
	    \rm *.o api/*.o parsers/xml/pugixml/src/*.o *~ index
