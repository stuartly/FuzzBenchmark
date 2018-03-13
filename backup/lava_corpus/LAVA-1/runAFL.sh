#!/bin/bash

for dir in file-5.22*
do
 (
  cd $dir
  mkdir Fuzz
  cp -r ../seed_dir Fuzz/
  afl-fuzz -i Fuzz/seed_dir -o Fuzz/output ./lava-install/bin/file @@
  )
done


