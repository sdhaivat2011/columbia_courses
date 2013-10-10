OBJS = index.o api/input.o parsers/xml/pugixml/src/pugixml.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

index : $(OBJS)
	    $(CC) $(LFLAGS) $(OBJS) -o index

index.o : index.cpp api/input.h parsers/json/cpp-json/json.h parsers/json/cpp-json/json/token.h parsers/json/cpp-json/json/exception.h parsers/json/cpp-json/json/object.h \
 parsers/json/cpp-json/json/array.h parsers/json/cpp-json/json/value.h parsers/json/cpp-json/json/json.tcc parsers/json/cpp-json/json/object.tcc parsers/json/cpp-json/json/array.tcc \
 parsers/json/cpp-json/json/value.tcc
	    $(CC) $(CFLAGS) index.cpp

api/input.o : api/input.h api/input.cpp parsers/xml/pugixml/src/pugixml.hpp
	    $(CC) $(CFLAGS) api/input.cpp -o api/input.o

parsers/xml/pugixml/src/pugixml.o : parsers/xml/pugixml/src/pugixml.hpp
	    $(CC) $(CFLAGS) parsers/xml/pugixml/src/pugixml.cpp -o parsers/xml/pugixml/src/pugixml.o

main.o: main.cc json.h json/token.h json/exception.h json/object.h \
 json/array.h json/value.h json/json.tcc json/object.tcc json/array.tcc \
 json/value.tcc


clean:
	    \rm *.o api/*.o parsers/xml/pugixml/src/*.o *~ index
