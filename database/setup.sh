#!/bin/sh
#
# Sets up the local database
#

sqlite3 /home/citizenwater/citizenwater.db << EOS
.read db_setup.sql
.exit
EOS

sqlite3 /home/citizenwater/test.db << EOS
.read db_setup.sql
.exit
EOS

gcc database-tester.c database.c -o database-tester -lsqlite3
gcc database-reader.c database.c -o database-reader -lsqlite3
