#!/bin/bash

for dir in file-5.22*
do
 (
  cd $dir
  mkdir Fuzz
  afl-fuzz -i ../seed_dir -o Fuzz/output ./lava-install/bin/file @@
  )
done


