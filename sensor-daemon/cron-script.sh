#!/bin/sh
#
# This script is invoked by cron and should run the sensor-daemon program.
# Should be placed in /opt/citwater/cron-script.sh

EXECPATH="/usr/bin/"
EXECNAME="sensor-daemon"

$EXECPATH/$EXECNAME
