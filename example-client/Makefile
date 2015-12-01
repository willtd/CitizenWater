# Makefile script for example-client

SHARED_FOLDER=$(shell pwd)/../shared
CONTROLLER_FOLDER=$(shell pwd)/../controller
INSTALL_PATH=/usr/bin


CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
LIBS=
SOURCES= example-client.c \
         $(CONTROLLER_FOLDER)/communication_controller.c \
         $(SHARED_FOLDER)/util.c \
         $(SHARED_FOLDER)/log.c  \
         $(SHARED_FOLDER)/sockets.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=citwater-client

.PHONY: all install clean

all: $(SOURCES) $(EXECUTABLE)

install: all
	cp $(EXECUTABLE) $(INSTALL_PATH)/$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
