#!/bin/bash
## TomHui created at Sun Dec 10 22:39:14 PST 2017

MYSQL=/usr/bin/mysql
#HOST=172.25.39.54
USER=tom
PSWD=tom123
#MSQLCMD="$MYSQL -h$HOST -u$USER -p$PSWD -B"
MSQLCMD="$MYSQL -u$USER -p$PSWD -B"


echo "DROP DATABASE laikelib_db" | $MSQLCMD

cat ./laikelib.sql
cat ./laikelib.sql      | $MSQLCMD


exit 0
