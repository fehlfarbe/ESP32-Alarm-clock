#!/bin/bash

./create_www.sh

echo "mklittlefs $@"
./mklittlefs "$@"