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
include tools/CMakeFiles/awa-server-write-attributes.dir/depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/awa-server-write-attributes.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/awa-server-write-attributes.dir/flags.make

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o: tools/CMakeFiles/awa-server-write-attributes.dir/flags.make
tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o: ../tools/awa-server-write-attributes.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-server-write-attributes.c

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-server-write-attributes.c > CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.i

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-server-write-attributes.c -o CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.s

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.requires:

.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.requires

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.provides: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.requires
	$(MAKE) -f tools/CMakeFiles/awa-server-write-attributes.dir/build.make tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.provides.build
.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.provides

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.provides.build: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o


tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o: tools/CMakeFiles/awa-server-write-attributes.dir/flags.make
tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o: ../tools/awa-server-write-attributes_cmdline.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -Wno-all -o CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-server-write-attributes_cmdline.c

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -Wno-all -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-server-write-attributes_cmdline.c > CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.i

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -Wno-all -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-server-write-attributes_cmdline.c -o CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.s

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.requires:

.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.requires

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.provides: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.requires
	$(MAKE) -f tools/CMakeFiles/awa-server-write-attributes.dir/build.make tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.provides.build
.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.provides

tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.provides.build: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o


# Object files for target awa-server-write-attributes
awa__server__write__attributes_OBJECTS = \
"CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o" \
"CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o"

# External object files for target awa-server-write-attributes
awa__server__write__attributes_EXTERNAL_OBJECTS = \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/Tools_object.dir/tools_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/Tools_object.dir/changeset_common.c.o"

tools/awa-server-write-attributes: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o
tools/awa-server-write-attributes: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o
tools/awa-server-write-attributes: tools/CMakeFiles/Tools_object.dir/tools_common.c.o
tools/awa-server-write-attributes: tools/CMakeFiles/Tools_object.dir/changeset_common.c.o
tools/awa-server-write-attributes: tools/CMakeFiles/awa-server-write-attributes.dir/build.make
tools/awa-server-write-attributes: api/src/libawa.a
tools/awa-server-write-attributes: lib/xml/libxml.a
tools/awa-server-write-attributes: lib/b64/libb64.a
tools/awa-server-write-attributes: lib/hmac/libhmac.a
tools/awa-server-write-attributes: tools/CMakeFiles/awa-server-write-attributes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable awa-server-write-attributes"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/awa-server-write-attributes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/CMakeFiles/awa-server-write-attributes.dir/build: tools/awa-server-write-attributes

.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/build

tools/CMakeFiles/awa-server-write-attributes.dir/requires: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes.c.o.requires
tools/CMakeFiles/awa-server-write-attributes.dir/requires: tools/CMakeFiles/awa-server-write-attributes.dir/awa-server-write-attributes_cmdline.c.o.requires

.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/requires

tools/CMakeFiles/awa-server-write-attributes.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && $(CMAKE_COMMAND) -P CMakeFiles/awa-server-write-attributes.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/clean

tools/CMakeFiles/awa-server-write-attributes.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/awa-server-write-attributes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/awa-server-write-attributes.dir/depend

