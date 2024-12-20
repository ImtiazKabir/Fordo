#!/bin/sh

if [ $# -ne 1 ]; then
  echo "Usage: $0 <database directory>"
  exit
fi

db=$1/fordo.db

if [ -f $db ]; then
  rm -i $db
fi

sqlite3 $db < $1/create_users_table.sql
sqlite3 $db < $1/create_todos_table.sql

echo "Database setup complete."
