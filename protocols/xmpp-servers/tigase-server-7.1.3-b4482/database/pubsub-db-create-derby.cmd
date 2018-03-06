@echo off

if [%1]==[] (
	echo. && echo Give me a path to the location where you have the database && echo.
  exit /b
)

set PWD="%cd%"

:: for tigase 5.0 and below
::java -Dij.protocol=jdbc:derby: -Dij.database="%1;create=true" ^
::		-Dderby.system.home=%PWD% ^
::		-classpath "libs/*" ^
::		org.apache.derby.tools.ij database/derby-schema-4.sql
::java -Dij.protocol=jdbc:derby: -Dij.database="%1" ^
::		-Dderby.system.home=%PWD% ^
::		-classpath "libs/*" ^
::		org.apache.derby.tools.ij database/derby-schema-4-sp.schema
::java -Dij.protocol=jdbc:derby: -Dij.database="%1" ^
::		-Dderby.system.home=%PWD% ^
::		-classpath "libs/*" ^
::		org.apache.derby.tools.ij database/derby-schema-4-props.sql

:: for Tigase 5.1

java -Dij.protocol=jdbc:derby: -Dij.database="%1;create=false" ^
		-Dderby.system.home=%PWD% ^
		-classpath jars/* ^
		org.apache.derby.tools.ij database/derby-pubsub-schema-3.0.0.sql > derby-db-import-pubsub.txt 2>&1

if %errorlevel% neq 0 (
  echo. && echo Error: please check the derby-db-import-pubsub.txt error file for more details && echo. && echo.
  exit /b %errorlevel%
) else (
  echo. && echo Success: please look at the derby-db-import-pubsub.txt file for more details && echo. && echo.
  echo configuration: && echo. && echo. && echo --comp-name-X=pubsub && echo. && echo --comp-class-X=tigase.pubsub.PubSubComponent && echo. && echo where 'X' is the next availiable index, starting from 1 && echo. && echo.
  exit /b
)

