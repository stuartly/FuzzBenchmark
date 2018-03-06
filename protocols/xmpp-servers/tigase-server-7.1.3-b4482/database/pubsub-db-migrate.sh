#!/bin/sh

LIB_DIR="jars"

CLASSPATH="`ls -d ${LIB_DIR}/*.jar 2>/dev/null | grep -v wrapper | tr '\n' :`${CLASSPATH}"

java -cp $CLASSPATH tigase.pubsub.repository.migration.Converter $@