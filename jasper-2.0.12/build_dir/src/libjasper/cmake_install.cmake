# Install script for directory: /home/cloud/work/jasper-2.0.12/src/libjasper

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/cloud/work/jasper-2.0.12/installed")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so.4.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so.4"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "/home/cloud/work/jasper-2.0.12/installed/lib")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/cloud/work/jasper-2.0.12/build_dir/src/libjasper/libjasper.so.4.0.0"
    "/home/cloud/work/jasper-2.0.12/build_dir/src/libjasper/libjasper.so.4"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so.4.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so.4"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "::::::::::::::::::::::::::::::::::::::::::::"
           NEW_RPATH "/home/cloud/work/jasper-2.0.12/installed/lib")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so"
         RPATH "/home/cloud/work/jasper-2.0.12/installed/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/cloud/work/jasper-2.0.12/build_dir/src/libjasper/libjasper.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so"
         OLD_RPATH "::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/home/cloud/work/jasper-2.0.12/installed/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libjasper.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/jasper" TYPE FILE FILES
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_cm.h"
    "/home/cloud/work/jasper-2.0.12/build_dir/src/libjasper/include/jasper/jas_config.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_debug.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_dll.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_fix.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_getopt.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_icc.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_image.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_init.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_malloc.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_math.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jasper.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_seq.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_stream.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_string.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_tmr.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_tvp.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_types.h"
    "/home/cloud/work/jasper-2.0.12/src/libjasper/include/jasper/jas_version.h"
    )
endif()

