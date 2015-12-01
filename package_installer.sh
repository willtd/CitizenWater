#!/bin/sh

# Necessary packages

sudo opkg update

# Crontab
sudo opkg install cronie

# Allows controller and clients to be run at boot time
sudo opkg install update-rc.d

# Database
sudo opkg install sqlite3
sudo opkg install libsqlite3
