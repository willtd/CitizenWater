#!/bin/sh
#
#

# Pull any new commits

git pull

# database
cd database && ./setup.sh && cd ..

# controller
cd controller && ./setup.sh && cd ..

# sensor-daemon
cd sensor-daemon && ./setup.sh && cd ..

# mraa-server
cd mraa-handler && ./setup.sh && cd ..

# clients

cd example-client && ./setup.sh && cd ..
cd bluetooth-client && ./setup.sh && cd ..

# customization files
cp -r default-customization-files/. ~
