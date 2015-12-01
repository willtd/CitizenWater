#!/bin/sh
#
# Cleans up all compiled code



# controller
cd controller
make clean 
cd ..

# sensor-daemon
cd sensor-daemon
make clean
cd ..

# mraa-server
cd mraa-handler
make clean
cd ..

# clients

cd example-client
make clean
cd ..

cd bluetooth-client
make clean
cd ..

