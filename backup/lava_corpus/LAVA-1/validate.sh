#!/bin/bash

echo "Creating directories for each bug..."
cat branches.txt | while read line ; do cp -r file-5.22/ file-5.22.${line} ; cd file-5.22.${line} ; git checkout ${line} ; cd .. ; done &> /dev/null

echo "Building buggy versions..."
cat branches.txt | xargs -P $(nproc) -n 1 ./conf.sh &> /dev/null

echo "Validating bugs..."
for d in file-5.22.* ; do
    { ${d}/lava-install/bin/file ${d}/CRASH_INPUT ; } &> /dev/null
    echo ${d} $?
done > validated.txt

awk 'BEGIN {valid = 0} $2 > 128 { valid += 1 } END { print "Validated", valid, "/", NR, "bugs" }' validated.txt

echo "You can see validated.txt for the exit code of each buggy version."
