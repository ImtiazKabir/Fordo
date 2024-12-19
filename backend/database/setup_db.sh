#!/bin/sh

sqlite3 fordo.db < create_users_table.sql
sqlite3 fordo.db < create_todos_table.sql
echo "Database setup complete."
