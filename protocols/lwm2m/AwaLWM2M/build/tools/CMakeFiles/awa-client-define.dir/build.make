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
include tools/CMakeFiles/awa-client-define.dir/depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/awa-client-define.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/awa-client-define.dir/flags.make

tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o: tools/CMakeFiles/awa-client-define.dir/flags.make
tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o: ../tools/awa-client-define.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa-client-define.dir/awa-client-define.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-client-define.c

tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa-client-define.dir/awa-client-define.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-client-define.c > CMakeFiles/awa-client-define.dir/awa-client-define.c.i

tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa-client-define.dir/awa-client-define.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-client-define.c -o CMakeFiles/awa-client-define.dir/awa-client-define.c.s

tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.requires:

.PHONY : tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.requires

tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.provides: tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.requires
	$(MAKE) -f tools/CMakeFiles/awa-client-define.dir/build.make tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.provides.build
.PHONY : tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.provides

tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.provides.build: tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o


tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o: tools/CMakeFiles/awa-client-define.dir/flags.make
tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o: ../tools/awa-client-define_cmdline.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -Wno-all -o CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-client-define_cmdline.c

tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -Wno-all -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-client-define_cmdline.c > CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.i

tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -Wno-all -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/awa-client-define_cmdline.c -o CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.s

tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.requires:

.PHONY : tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.requires

tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.provides: tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.requires
	$(MAKE) -f tools/CMakeFiles/awa-client-define.dir/build.make tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.provides.build
.PHONY : tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.provides

tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.provides.build: tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o


tools/CMakeFiles/awa-client-define.dir/define_common.c.o: tools/CMakeFiles/awa-client-define.dir/flags.make
tools/CMakeFiles/awa-client-define.dir/define_common.c.o: ../tools/define_common.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object tools/CMakeFiles/awa-client-define.dir/define_common.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa-client-define.dir/define_common.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/define_common.c

tools/CMakeFiles/awa-client-define.dir/define_common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa-client-define.dir/define_common.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/define_common.c > CMakeFiles/awa-client-define.dir/define_common.c.i

tools/CMakeFiles/awa-client-define.dir/define_common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa-client-define.dir/define_common.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools/define_common.c -o CMakeFiles/awa-client-define.dir/define_common.c.s

tools/CMakeFiles/awa-client-define.dir/define_common.c.o.requires:

.PHONY : tools/CMakeFiles/awa-client-define.dir/define_common.c.o.requires

tools/CMakeFiles/awa-client-define.dir/define_common.c.o.provides: tools/CMakeFiles/awa-client-define.dir/define_common.c.o.requires
	$(MAKE) -f tools/CMakeFiles/awa-client-define.dir/build.make tools/CMakeFiles/awa-client-define.dir/define_common.c.o.provides.build
.PHONY : tools/CMakeFiles/awa-client-define.dir/define_common.c.o.provides

tools/CMakeFiles/awa-client-define.dir/define_common.c.o.provides.build: tools/CMakeFiles/awa-client-define.dir/define_common.c.o


# Object files for target awa-client-define
awa__client__define_OBJECTS = \
"CMakeFiles/awa-client-define.dir/awa-client-define.c.o" \
"CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o" \
"CMakeFiles/awa-client-define.dir/define_common.c.o"

# External object files for target awa-client-define
awa__client__define_EXTERNAL_OBJECTS = \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/Tools_object.dir/tools_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/Tools_object.dir/changeset_common.c.o"

tools/awa-client-define: tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o
tools/awa-client-define: tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o
tools/awa-client-define: tools/CMakeFiles/awa-client-define.dir/define_common.c.o
tools/awa-client-define: tools/CMakeFiles/Tools_object.dir/tools_common.c.o
tools/awa-client-define: tools/CMakeFiles/Tools_object.dir/changeset_common.c.o
tools/awa-client-define: tools/CMakeFiles/awa-client-define.dir/build.make
tools/awa-client-define: api/src/libawa.a
tools/awa-client-define: lib/xml/libxml.a
tools/awa-client-define: lib/hmac/libhmac.a
tools/awa-client-define: lib/b64/libb64.a
tools/awa-client-define: tools/CMakeFiles/awa-client-define.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable awa-client-define"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/awa-client-define.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/CMakeFiles/awa-client-define.dir/build: tools/awa-client-define

.PHONY : tools/CMakeFiles/awa-client-define.dir/build

tools/CMakeFiles/awa-client-define.dir/requires: tools/CMakeFiles/awa-client-define.dir/awa-client-define.c.o.requires
tools/CMakeFiles/awa-client-define.dir/requires: tools/CMakeFiles/awa-client-define.dir/awa-client-define_cmdline.c.o.requires
tools/CMakeFiles/awa-client-define.dir/requires: tools/CMakeFiles/awa-client-define.dir/define_common.c.o.requires

.PHONY : tools/CMakeFiles/awa-client-define.dir/requires

tools/CMakeFiles/awa-client-define.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools && $(CMAKE_COMMAND) -P CMakeFiles/awa-client-define.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/awa-client-define.dir/clean

tools/CMakeFiles/awa-client-define.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/tools /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/tools/CMakeFiles/awa-client-define.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/awa-client-define.dir/depend

