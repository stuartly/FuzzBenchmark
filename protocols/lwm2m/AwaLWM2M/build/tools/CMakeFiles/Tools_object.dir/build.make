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
include tools/CMakeFiles/Tools_object.dir/depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/Tools_object.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/Tools_object.dir/flags.make

tools/CMakeFiles/Tools_object.dir/tools_common.c.o: tools/CMakeFiles/Tools_object.dir/flags.make
tools/CMakeFiles/Tools_object.dir/tools_common.c.o: ../tools/tools_common.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tools/CMakeFiles/Tools_object.dir/tools_common.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Tools_object.dir/tools_common.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/tools_common.c

tools/CMakeFiles/Tools_object.dir/tools_common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Tools_object.dir/tools_common.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/tools_common.c > CMakeFiles/Tools_object.dir/tools_common.c.i

tools/CMakeFiles/Tools_object.dir/tools_common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Tools_object.dir/tools_common.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/tools_common.c -o CMakeFiles/Tools_object.dir/tools_common.c.s

tools/CMakeFiles/Tools_object.dir/tools_common.c.o.requires:

.PHONY : tools/CMakeFiles/Tools_object.dir/tools_common.c.o.requires

tools/CMakeFiles/Tools_object.dir/tools_common.c.o.provides: tools/CMakeFiles/Tools_object.dir/tools_common.c.o.requires
	$(MAKE) -f tools/CMakeFiles/Tools_object.dir/build.make tools/CMakeFiles/Tools_object.dir/tools_common.c.o.provides.build
.PHONY : tools/CMakeFiles/Tools_object.dir/tools_common.c.o.provides

tools/CMakeFiles/Tools_object.dir/tools_common.c.o.provides.build: tools/CMakeFiles/Tools_object.dir/tools_common.c.o


tools/CMakeFiles/Tools_object.dir/changeset_common.c.o: tools/CMakeFiles/Tools_object.dir/flags.make
tools/CMakeFiles/Tools_object.dir/changeset_common.c.o: ../tools/changeset_common.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object tools/CMakeFiles/Tools_object.dir/changeset_common.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Tools_object.dir/changeset_common.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/changeset_common.c

tools/CMakeFiles/Tools_object.dir/changeset_common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Tools_object.dir/changeset_common.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/changeset_common.c > CMakeFiles/Tools_object.dir/changeset_common.c.i

tools/CMakeFiles/Tools_object.dir/changeset_common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Tools_object.dir/changeset_common.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/changeset_common.c -o CMakeFiles/Tools_object.dir/changeset_common.c.s

tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.requires:

.PHONY : tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.requires

tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.provides: tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.requires
	$(MAKE) -f tools/CMakeFiles/Tools_object.dir/build.make tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.provides.build
.PHONY : tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.provides

tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.provides.build: tools/CMakeFiles/Tools_object.dir/changeset_common.c.o


Tools_object: tools/CMakeFiles/Tools_object.dir/tools_common.c.o
Tools_object: tools/CMakeFiles/Tools_object.dir/changeset_common.c.o
Tools_object: tools/CMakeFiles/Tools_object.dir/build.make

.PHONY : Tools_object

# Rule to build all files generated by this target.
tools/CMakeFiles/Tools_object.dir/build: Tools_object

.PHONY : tools/CMakeFiles/Tools_object.dir/build

tools/CMakeFiles/Tools_object.dir/requires: tools/CMakeFiles/Tools_object.dir/tools_common.c.o.requires
tools/CMakeFiles/Tools_object.dir/requires: tools/CMakeFiles/Tools_object.dir/changeset_common.c.o.requires

.PHONY : tools/CMakeFiles/Tools_object.dir/requires

tools/CMakeFiles/Tools_object.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && $(CMAKE_COMMAND) -P CMakeFiles/Tools_object.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/Tools_object.dir/clean

tools/CMakeFiles/Tools_object.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/Tools_object.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/Tools_object.dir/depend
