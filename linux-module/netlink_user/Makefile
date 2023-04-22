CC=g++
CFLAGS=-c -Wall
INCDIRS=-I/usr/include/libnl3
LDFLAGS=-lnl-genl-3 -lnl-3
SOURCES=test_netlink.c
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=test_netlink

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(INCDIRS) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(INCDIRS) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)
