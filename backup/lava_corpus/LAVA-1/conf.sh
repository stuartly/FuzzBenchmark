#!/bin/bash

for dir in file-5.22*
do 
  (
  echo $dir 
  cd $dir
  make clean
  make uninstall
  autoreconf -f -i
  CC=afl-clang-fast ./configure --enable-static --disable-shared --prefix=$PWD/lava-install CFLAGS="-fvisibility=default -ggdb"
  make && make install
  )
done
  


