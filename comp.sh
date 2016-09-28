#!/bin/sh

g++ "$1".cc -o "$1".out /usr/local/lib/libleveldb.a  -lpthread -I /usr/local/include/leveldb/
