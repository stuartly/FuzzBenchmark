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
include api/src/CMakeFiles/Awa_shared.dir/depend.make

# Include the progress variables for this target.
include api/src/CMakeFiles/Awa_shared.dir/progress.make

# Include the compile flags for this target's objects.
include api/src/CMakeFiles/Awa_shared.dir/flags.make

# Object files for target Awa_shared
Awa_shared_OBJECTS =

# External object files for target Awa_shared
Awa_shared_EXTERNAL_OBJECTS = \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/log.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/error.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/lwm2m_error.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/ipc.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/utils.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/changeset.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_subscribe_response.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/response_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_response.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/iterator.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/string_iterator.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/path_iterator.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_iterator.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/registered_entity_iterator.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/path.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/path_result.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/arrays.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/queue.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/list.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/value.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/objects_tree.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/define_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/integer_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/float_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/string_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/opaque_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/time_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/boolean_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/objectlink_array.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/map.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/session_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_session.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_session.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/operation_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/set_write_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/define_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_define_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_define_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/set_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/get_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_delete_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_delete_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/get_response.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_subscribe.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/client_notification.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_notification.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/observe_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/list_clients_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/write_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/write_mode.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/read_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/subscribe_observe_common.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/write_attributes_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/execute_operation.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/server_events.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/unsupported.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/daemon/src/common/xml.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_definition.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_list.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_types.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_result.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_debug.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_tree_node.c.o" \
"/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_object.dir/__/__/daemon/src/common/lwm2m_xml_serdes.c.o"

api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/log.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/error.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/lwm2m_error.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/ipc.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/utils.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/changeset.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_subscribe_response.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/response_common.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_response.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/iterator.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/string_iterator.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/path_iterator.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_iterator.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/registered_entity_iterator.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/path.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/path_result.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/arrays.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/queue.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/list.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/value.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/objects_tree.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/define_common.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/integer_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/float_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/string_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/opaque_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/time_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/boolean_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/objectlink_array.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/map.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/session_common.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_session.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_session.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/operation_common.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/set_write_common.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/define_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_define_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_define_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/set_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/get_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_delete_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_delete_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/get_response.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_subscribe.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/client_notification.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_notification.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/observe_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/list_clients_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/write_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/write_mode.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/read_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/subscribe_observe_common.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/write_attributes_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/execute_operation.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/server_events.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/unsupported.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/daemon/src/common/xml.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_definition.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_list.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_types.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_result.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_debug.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/core/src/common/lwm2m_tree_node.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_object.dir/__/__/daemon/src/common/lwm2m_xml_serdes.c.o
api/src/libawa.so: api/src/CMakeFiles/Awa_shared.dir/build.make
api/src/libawa.so: lib/xml/libxml.a
api/src/libawa.so: lib/b64/libb64.a
api/src/libawa.so: lib/hmac/libhmac.a
api/src/libawa.so: api/src/CMakeFiles/Awa_shared.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking C shared library libawa.so"
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Awa_shared.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
api/src/CMakeFiles/Awa_shared.dir/build: api/src/libawa.so

.PHONY : api/src/CMakeFiles/Awa_shared.dir/build

api/src/CMakeFiles/Awa_shared.dir/requires:

.PHONY : api/src/CMakeFiles/Awa_shared.dir/requires

api/src/CMakeFiles/Awa_shared.dir/clean:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src && $(CMAKE_COMMAND) -P CMakeFiles/Awa_shared.dir/cmake_clean.cmake
.PHONY : api/src/CMakeFiles/Awa_shared.dir/clean

api/src/CMakeFiles/Awa_shared.dir/depend:
	cd /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/api/src /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src /home/lzs243/Documents/ProtocolFuzz/Benchmark/AwaLWM2M/build/api/src/CMakeFiles/Awa_shared.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : api/src/CMakeFiles/Awa_shared.dir/depend
