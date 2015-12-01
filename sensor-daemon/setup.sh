#!/bin/sh
#
# Sets up the sensor-daemon module


# Compile
make
# gcc -Wall -lmraa sensor-daemon-main.c ../shared/log.c ../shared/util.c ../atlsci-sensors/sensors.c -o $SENSOR_EXE_NAME

