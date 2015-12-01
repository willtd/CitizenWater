#!/bin/sh
#
# Run this script to install the CitizenWater product.

# update-rc.d is in /usr/sbin, which is not part of the PATH by default
PATH=$PATH:/usr/sbin
export PATH

sudo ./package_installer.sh

./setup.sh && sudo ./root_setup.sh
