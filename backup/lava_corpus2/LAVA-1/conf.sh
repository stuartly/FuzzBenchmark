#!/bin/bash

dir="file-5.22.${1}"
cd "$dir"
autoreconf -f -i
CC=afl-clang-fast ./configure --enable-static --disable-shared --prefix=`pwd`/lava-install CFLAGS="-fvisibility=default -ggdb"
make 
sudo make install
