#!/bin/sh
#
# Sets up the local database
#

sqlite3 citizen.db << EOS
.read android_db_setup.sql
.exit
EOS
