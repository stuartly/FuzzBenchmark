#!/bin/bash

for dir in file-5.22*
do 
  (
  echo $dir 
  cd $dir
  cp ../runAFL.sh .
  )
done
  


