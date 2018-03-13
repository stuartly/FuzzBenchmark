#!/bin/bash
 autoreconf -f -i
 CC = afl-clang-fast ./configure --enable-static --disable-shared --prefix=`pwd`/lava-install
 make && make install
