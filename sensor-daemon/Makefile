# Makefile script for sensor-daemon

SHARED_FOLDER=$(shell pwd)/../shared
JSON_FOLDER=$(shell pwd)/../cJSON
SENSOR_FOLDER=$(shell pwd)/../atlsci-sensors
SQLITE_FOLDER=$(shell pwd)/../database

INSTALL_PATH=/usr/bin

CRON_SCRIPT_PATH=/opt/citwater
CRON_SCRIPT=cron-script.sh

CC=gcc
CFLAGS=-c -Wall 
LDFLAGS= 
LIBS=-lmraa -lsqlite3 -ldl -lpthread -lm
SOURCES= sensor-daemon-main.c \
         daemon_options.c \
         $(SHARED_FOLDER)/util.c \
         $(SHARED_FOLDER)/log.c  \
         $(SENSOR_FOLDER)/sensors.c \
	     $(JSON_FOLDER)/cJSON.c\
	     $(SQLITE_FOLDER)/database.c \
         $(SHARED_FOLDER)/sockets.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=sensor-daemon

.PHONY: all install clean

all: $(SOURCES) $(EXECUTABLE)

install: all
	cp $(EXECUTABLE) $(INSTALL_PATH)/$(EXECUTABLE)
	mkdir -p $(CRON_SCRIPT_PATH)
	cp $(CRON_SCRIPT) $(CRON_SCRIPT_PATH)/$(CRON_SCRIPT)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
