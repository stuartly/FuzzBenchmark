# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build

# Include any dependencies generated for this target.
include core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/depend.make

# Include the progress variables for this target.
include core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/progress.make

# Include the compile flags for this target's objects.
include core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/flags.make

# Object files for target awa_bootstrap_server_shared
awa_bootstrap_server_shared_OBJECTS =

# External object files for target awa_bootstrap_server_shared
awa_bootstrap_server_shared_EXTERNAL_OBJECTS = \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/lwm2m_bootstrap.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/lwm2m_bootstrap_core.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/server/lwm2m_object_defs.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_bootstrap_config.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_serdes.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_tlv.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_plaintext.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_prettyprint.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_opaque.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_tree_builder.c.o"

core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/lwm2m_bootstrap.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/lwm2m_bootstrap_core.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/server/lwm2m_object_defs.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_bootstrap_config.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_serdes.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_tlv.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_plaintext.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_prettyprint.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_opaque.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_object.dir/__/common/lwm2m_tree_builder.c.o
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/build.make
core/src/bootstrap/liblwm2mbootstrapserver.so: core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking C shared library liblwm2mbootstrapserver.so"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/awa_bootstrap_server_shared.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/build: core/src/bootstrap/liblwm2mbootstrapserver.so

.PHONY : core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/build

core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/requires:

.PHONY : core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/requires

core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap && $(CMAKE_COMMAND) -P CMakeFiles/awa_bootstrap_server_shared.dir/cmake_clean.cmake
.PHONY : core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/clean

core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/bootstrap /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : core/src/bootstrap/CMakeFiles/awa_bootstrap_server_shared.dir/depend

