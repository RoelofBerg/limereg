#!/bin/sh
./autoclean.sh
./autogen.sh
./configure CFLAGS="-Ofast" CXXFLAGS="-Ofast"
make distcheck
