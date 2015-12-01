#!/bin/sh
#
# This script runs the controller on startup of the CPU. It should be 
# placed in the directory /etc/init.d.
#
# The controller is run as the user 'citizenwater'.
#

su - citizenwater -c 'citwater-controller --log-level=0'
exit

