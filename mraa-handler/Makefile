# Makefile script for controller

SHARED_FOLDER=$(shell pwd)/../shared

INSTALL_PATH=/usr/bin
STARTUP_SCRIPT_NAME=mraa-handler-startup.sh
CC=gcc
CFLAGS=-c -Wall 
LDFLAGS=
LIBS=-lmraa 
SOURCES= mraa-handler.c \
         $(SHARED_FOLDER)/log.c  \
         $(SHARED_FOLDER)/sockets.c \

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mraa-handler

.PHONY: all install clean

all: $(SOURCES) $(EXECUTABLE)

install: all
	echo "SUBSYSTEM==\"gpio\", ACTION==\"add\", PROGRAM=\"/bin/sh -c 'chown -R root:gpio /sys%p; chown -R root:gpio /sys/class/gpio; chmod -R 775 /sys%p; chmod -R 775 /sys/class/gpio; chown -R root:gpio /dev/i2c-6; chmod -R 775 /dev/i2c-6; ./home/citizenwater/asu_capstone/mraa-handler/sensors_setup'\" "> /etc/udev/rules.d/99-gpio.rules
	/usr/sbin/groupadd -f --system gpio
	udevadm control --reload-rules
	gpasswd -a citizenwater gpio

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
