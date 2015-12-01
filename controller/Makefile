# Makefile script for controller

SHARED_FOLDER=$(shell pwd)/../shared
JSON_FOLDER=$(shell pwd)/../cJSON
CRON_FOLDER=$(shell pwd)/../cron
SENSOR_FOLDER=$(shell pwd)/../atlsci-sensors
SQLITE_FOLDER=$(shell pwd)/../database
INSTALL_PATH=/usr/bin
STARTUP_SCRIPT_NAME=citwater-startup.sh
CC=gcc
CFLAGS=-c -Wall 
LDFLAGS=
LIBS=-lm -lmraa -lpthread -ldl -lsqlite3
SOURCES= main.c \
         options.c \
         communication_controller.c \
         handler.c \
         sensor_handlers.c \
         $(SHARED_FOLDER)/util.c \
         $(SHARED_FOLDER)/log.c  \
         $(SHARED_FOLDER)/sockets.c \
         $(JSON_FOLDER)/cJSON.c \
         $(CRON_FOLDER)/cron.c \
         $(CRON_FOLDER)/cron_json.c \
	     $(SENSOR_FOLDER)/sensors.c \
	     $(SENSOR_FOLDER)/Adafruit_MAX31855.c \
	     $(SQLITE_FOLDER)/database.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=citwater-controller

.PHONY: all install clean

all: $(SOURCES) $(EXECUTABLE)

install: all
	cp $(EXECUTABLE) $(INSTALL_PATH)/$(EXECUTABLE)
	mkdir -p /etc/init.d
	cp $(STARTUP_SCRIPT_NAME) /etc/init.d/$(STARTUP_SCRIPT_NAME)
	/usr/sbin/update-rc.d $(STARTUP_SCRIPT_NAME) defaults
	chmod 4774 /var/spool/cron # drwsrwxr--, ensure permissions are correct
	gpasswd -a citizenwater crontab # add to group
	# update-rc.d crond defaults

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
