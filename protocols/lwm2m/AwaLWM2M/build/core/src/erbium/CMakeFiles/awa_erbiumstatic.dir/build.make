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
include core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/depend.make

# Include the progress variables for this target.
include core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/progress.make

# Include the compile flags for this target's objects.
include core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/flags.make

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/flags.make
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o: ../core/src/erbium/er-coap.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap.c

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa_erbiumstatic.dir/er-coap.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap.c > CMakeFiles/awa_erbiumstatic.dir/er-coap.c.i

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa_erbiumstatic.dir/er-coap.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap.c -o CMakeFiles/awa_erbiumstatic.dir/er-coap.c.s

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.requires:

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.requires

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.provides: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.requires
	$(MAKE) -f core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build.make core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.provides.build
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.provides

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.provides.build: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o


core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/flags.make
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o: ../core/src/erbium/er-coap-engine.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-engine.c

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-engine.c > CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.i

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-engine.c -o CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.s

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.requires:

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.requires

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.provides: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.requires
	$(MAKE) -f core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build.make core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.provides.build
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.provides

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.provides.build: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o


core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/flags.make
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o: ../core/src/erbium/er-coap-transactions.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-transactions.c

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-transactions.c > CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.i

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-transactions.c -o CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.s

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.requires:

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.requires

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.provides: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.requires
	$(MAKE) -f core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build.make core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.provides.build
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.provides

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.provides.build: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o


core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/flags.make
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o: ../core/src/erbium/er-coap-separate.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-separate.c

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-separate.c > CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.i

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-separate.c -o CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.s

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.requires:

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.requires

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.provides: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.requires
	$(MAKE) -f core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build.make core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.provides.build
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.provides

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.provides.build: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o


core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/flags.make
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o: ../core/src/erbium/er-coap-block1.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o   -c /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-block1.c

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.i"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-block1.c > CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.i

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.s"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium/er-coap-block1.c -o CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.s

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.requires:

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.requires

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.provides: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.requires
	$(MAKE) -f core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build.make core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.provides.build
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.provides

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.provides.build: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o


# Object files for target awa_erbiumstatic
awa_erbiumstatic_OBJECTS = \
"CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o" \
"CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o" \
"CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o" \
"CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o" \
"CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o"

# External object files for target awa_erbiumstatic
awa_erbiumstatic_EXTERNAL_OBJECTS =

core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o
core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o
core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o
core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o
core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o
core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build.make
core/src/erbium/liberbiumstatic.a: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C static library liberbiumstatic.a"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && $(CMAKE_COMMAND) -P CMakeFiles/awa_erbiumstatic.dir/cmake_clean_target.cmake
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/awa_erbiumstatic.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build: core/src/erbium/liberbiumstatic.a

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/build

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/requires: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap.c.o.requires
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/requires: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-engine.c.o.requires
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/requires: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-transactions.c.o.requires
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/requires: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-separate.c.o.requires
core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/requires: core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/er-coap-block1.c.o.requires

.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/requires

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium && $(CMAKE_COMMAND) -P CMakeFiles/awa_erbiumstatic.dir/cmake_clean.cmake
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/clean

core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/core/src/erbium /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : core/src/erbium/CMakeFiles/awa_erbiumstatic.dir/depend

