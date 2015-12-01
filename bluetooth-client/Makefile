# Makefile script for Bluetooth client

SHARED_FOLDER=$(shell pwd)/../shared
JSON_FOLDER=$(shell pwd)/../cJSON
CONTROLLER_FOLDER=$(shell pwd)/../controller
DATABASE_FOLDER=$(shell pwd)/../database

INSTALL_PATH=/usr/bin

STARTUP_SCRIPT_NAME=citwater-bluetooth-startup.sh
CC=gcc 
CFLAGS=-c -Wall
LDFLAGS=
LIBS=-lm -lbluetooth -lsqlite3
SOURCES= bluetooth-client.c \
	 bluetooth-citizen.c \
         $(SHARED_FOLDER)/log.c \
         $(SHARED_FOLDER)/sockets.c \
         $(CONTROLLER_FOLDER)/communication_controller.c \
         $(DATABASE_FOLDER)/database.c \
	     $(JSON_FOLDER)/cJSON.c\

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=citwater-bluetooth
TESTSOURCES= test_bluetooth-client.c \
	 bluetooth-citizen.c \
         $(SHARED_FOLDER)/log.c \
         $(SHARED_FOLDER)/sockets.c \
         $(CONTROLLER_FOLDER)/communication_controller.c \
         $(DATABASE_FOLDER)/database.c \
	     $(JSON_FOLDER)/cJSON.c\

TESTOBJECTS=$(TESTSOURCES:.c=.o)
TESTEXECUTABLE=test_citwater-bluetooth


.PHONY: all install clean

all: program testprogram
program: $(SOURCES) $(EXECUTABLE)
testprogram: $(TESTSOURCES) $(TESTEXECUTABLE)
install: all
	cp $(EXECUTABLE) $(INSTALL_PATH)/$(EXECUTABLE)
	mkdir -p /etc/init.d
	cp citwater-bluetooth.service /lib/systemd/system/citwater-bluetooth.service
	cp citwater-bluetooth-startup.sh /opt/citwater/citwater-bluetooth-startup.sh
	cp citwater-bluetooth-client-startup.sh /opt/citwater/citwater-bluetooth-client-startup.sh
	cp simple-agent.py /opt/citwater/simple-agent.py
	cp bluezutils.py /opt/citwater/bluezutils.py
	cp bluetooth.service /lib/systemd/system/bluetooth.service
	cp citwater-bluetooth-client.service /lib/systemd/system/citwater-bluetooth-client.service
	systemctl daemon-reload
	systemctl enable bluetooth
	systemctl enable citwater-bluetooth
	systemctl enable citwater-bluetooth-client

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

$(TESTEXECUTABLE): $(TESTOBJECTS)
	$(CC) $(LDFLAGS) $(TESTOBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
