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
include api/examples/CMakeFiles/server-delete-object-instance-example.dir/depend.make

# Include the progress variables for this target.
include api/examples/CMakeFiles/server-delete-object-instance-example.dir/progress.make

# Include the compile flags for this target's objects.
include api/examples/CMakeFiles/server-delete-object-instance-example.dir/flags.make

api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o: api/examples/CMakeFiles/server-delete-object-instance-example.dir/flags.make
api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o: ../api/examples/server-delete-object-instance-example.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples/server-delete-object-instance-example.c

api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples/server-delete-object-instance-example.c > CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.i

api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples/server-delete-object-instance-example.c -o CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.s

api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.requires:

.PHONY : api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.requires

api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.provides: api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.requires
	$(MAKE) -f api/examples/CMakeFiles/server-delete-object-instance-example.dir/build.make api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.provides.build
.PHONY : api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.provides

api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.provides.build: api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o


# Object files for target server-delete-object-instance-example
server__delete__object__instance__example_OBJECTS = \
"CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o"

# External object files for target server-delete-object-instance-example
server__delete__object__instance__example_EXTERNAL_OBJECTS =

api/examples/server-delete-object-instance-example: api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o
api/examples/server-delete-object-instance-example: api/examples/CMakeFiles/server-delete-object-instance-example.dir/build.make
api/examples/server-delete-object-instance-example: api/src/libawa.a
api/examples/server-delete-object-instance-example: lib/xml/libxml.a
api/examples/server-delete-object-instance-example: lib/b64/libb64.a
api/examples/server-delete-object-instance-example: lib/hmac/libhmac.a
api/examples/server-delete-object-instance-example: api/examples/CMakeFiles/server-delete-object-instance-example.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable server-delete-object-instance-example"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server-delete-object-instance-example.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
api/examples/CMakeFiles/server-delete-object-instance-example.dir/build: api/examples/server-delete-object-instance-example

.PHONY : api/examples/CMakeFiles/server-delete-object-instance-example.dir/build

api/examples/CMakeFiles/server-delete-object-instance-example.dir/requires: api/examples/CMakeFiles/server-delete-object-instance-example.dir/server-delete-object-instance-example.c.o.requires

.PHONY : api/examples/CMakeFiles/server-delete-object-instance-example.dir/requires

api/examples/CMakeFiles/server-delete-object-instance-example.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && $(CMAKE_COMMAND) -P CMakeFiles/server-delete-object-instance-example.dir/cmake_clean.cmake
.PHONY : api/examples/CMakeFiles/server-delete-object-instance-example.dir/clean

api/examples/CMakeFiles/server-delete-object-instance-example.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples/CMakeFiles/server-delete-object-instance-example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : api/examples/CMakeFiles/server-delete-object-instance-example.dir/depend

