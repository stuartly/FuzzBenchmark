#!/bin/bash

dir="file-5.22.${1}"
cd "$dir"
autoreconf -f -i
./configure --enable-static --disable-shared --prefix=`pwd`/lava-install CFLAGS="-fvisibility=default -ggdb"
make && make install
