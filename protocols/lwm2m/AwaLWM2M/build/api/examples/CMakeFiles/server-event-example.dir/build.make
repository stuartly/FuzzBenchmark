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
include api/examples/CMakeFiles/server-event-example.dir/depend.make

# Include the progress variables for this target.
include api/examples/CMakeFiles/server-event-example.dir/progress.make

# Include the compile flags for this target's objects.
include api/examples/CMakeFiles/server-event-example.dir/flags.make

api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o: api/examples/CMakeFiles/server-event-example.dir/flags.make
api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o: ../api/examples/server-event-example.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/server-event-example.dir/server-event-example.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples/server-event-example.c

api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/server-event-example.dir/server-event-example.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples/server-event-example.c > CMakeFiles/server-event-example.dir/server-event-example.c.i

api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/server-event-example.dir/server-event-example.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples/server-event-example.c -o CMakeFiles/server-event-example.dir/server-event-example.c.s

api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.requires:

.PHONY : api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.requires

api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.provides: api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.requires
	$(MAKE) -f api/examples/CMakeFiles/server-event-example.dir/build.make api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.provides.build
.PHONY : api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.provides

api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.provides.build: api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o


# Object files for target server-event-example
server__event__example_OBJECTS = \
"CMakeFiles/server-event-example.dir/server-event-example.c.o"

# External object files for target server-event-example
server__event__example_EXTERNAL_OBJECTS =

api/examples/server-event-example: api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o
api/examples/server-event-example: api/examples/CMakeFiles/server-event-example.dir/build.make
api/examples/server-event-example: api/src/libawa.a
api/examples/server-event-example: lib/xml/libxml.a
api/examples/server-event-example: lib/b64/libb64.a
api/examples/server-event-example: lib/hmac/libhmac.a
api/examples/server-event-example: api/examples/CMakeFiles/server-event-example.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable server-event-example"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server-event-example.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
api/examples/CMakeFiles/server-event-example.dir/build: api/examples/server-event-example

.PHONY : api/examples/CMakeFiles/server-event-example.dir/build

api/examples/CMakeFiles/server-event-example.dir/requires: api/examples/CMakeFiles/server-event-example.dir/server-event-example.c.o.requires

.PHONY : api/examples/CMakeFiles/server-event-example.dir/requires

api/examples/CMakeFiles/server-event-example.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples && $(CMAKE_COMMAND) -P CMakeFiles/server-event-example.dir/cmake_clean.cmake
.PHONY : api/examples/CMakeFiles/server-event-example.dir/clean

api/examples/CMakeFiles/server-event-example.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/examples /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/examples/CMakeFiles/server-event-example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : api/examples/CMakeFiles/server-event-example.dir/depend

