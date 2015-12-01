#!/bin/sh
#
# This is run as root

# controller
cd controller && ./root_setup.sh && cd ..

# sensor-daemon
cd sensor-daemon && ./root_setup.sh && cd ..

# clients
cd bluetooth-client && ./root_setup.sh && cd ..
cd example-client && ./root_setup.sh && cd ..

# mraa-server
cd mraa-handler && ./root_setup.sh && cd ..
