# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/artem/Documents/ASIO_PRACTICE

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/artem/Documents/ASIO_PRACTICE/build

# Include any dependencies generated for this target.
include FServer2_0/CMakeFiles/tests_server.dir/depend.make

# Include the progress variables for this target.
include FServer2_0/CMakeFiles/tests_server.dir/progress.make

# Include the compile flags for this target's objects.
include FServer2_0/CMakeFiles/tests_server.dir/flags.make

FServer2_0/CMakeFiles/tests_server.dir/testing/tests_server.cpp.o: FServer2_0/CMakeFiles/tests_server.dir/flags.make
FServer2_0/CMakeFiles/tests_server.dir/testing/tests_server.cpp.o: ../FServer2_0/testing/tests_server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/artem/Documents/ASIO_PRACTICE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object FServer2_0/CMakeFiles/tests_server.dir/testing/tests_server.cpp.o"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tests_server.dir/testing/tests_server.cpp.o -c /home/artem/Documents/ASIO_PRACTICE/FServer2_0/testing/tests_server.cpp

FServer2_0/CMakeFiles/tests_server.dir/testing/tests_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tests_server.dir/testing/tests_server.cpp.i"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/artem/Documents/ASIO_PRACTICE/FServer2_0/testing/tests_server.cpp > CMakeFiles/tests_server.dir/testing/tests_server.cpp.i

FServer2_0/CMakeFiles/tests_server.dir/testing/tests_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tests_server.dir/testing/tests_server.cpp.s"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/artem/Documents/ASIO_PRACTICE/FServer2_0/testing/tests_server.cpp -o CMakeFiles/tests_server.dir/testing/tests_server.cpp.s

FServer2_0/CMakeFiles/tests_server.dir/source/Server/Server.cpp.o: FServer2_0/CMakeFiles/tests_server.dir/flags.make
FServer2_0/CMakeFiles/tests_server.dir/source/Server/Server.cpp.o: ../FServer2_0/source/Server/Server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/artem/Documents/ASIO_PRACTICE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object FServer2_0/CMakeFiles/tests_server.dir/source/Server/Server.cpp.o"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tests_server.dir/source/Server/Server.cpp.o -c /home/artem/Documents/ASIO_PRACTICE/FServer2_0/source/Server/Server.cpp

FServer2_0/CMakeFiles/tests_server.dir/source/Server/Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tests_server.dir/source/Server/Server.cpp.i"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/artem/Documents/ASIO_PRACTICE/FServer2_0/source/Server/Server.cpp > CMakeFiles/tests_server.dir/source/Server/Server.cpp.i

FServer2_0/CMakeFiles/tests_server.dir/source/Server/Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tests_server.dir/source/Server/Server.cpp.s"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/artem/Documents/ASIO_PRACTICE/FServer2_0/source/Server/Server.cpp -o CMakeFiles/tests_server.dir/source/Server/Server.cpp.s

# Object files for target tests_server
tests_server_OBJECTS = \
"CMakeFiles/tests_server.dir/testing/tests_server.cpp.o" \
"CMakeFiles/tests_server.dir/source/Server/Server.cpp.o"

# External object files for target tests_server
tests_server_EXTERNAL_OBJECTS =

FServer2_0/tests_server: FServer2_0/CMakeFiles/tests_server.dir/testing/tests_server.cpp.o
FServer2_0/tests_server: FServer2_0/CMakeFiles/tests_server.dir/source/Server/Server.cpp.o
FServer2_0/tests_server: FServer2_0/CMakeFiles/tests_server.dir/build.make
FServer2_0/tests_server: lib/libgtestd.a
FServer2_0/tests_server: lib/libgtest_maind.a
FServer2_0/tests_server: lib/libgmockd.a
FServer2_0/tests_server: lib/libgmock_maind.a
FServer2_0/tests_server: lib/libgmockd.a
FServer2_0/tests_server: lib/libgtestd.a
FServer2_0/tests_server: FServer2_0/CMakeFiles/tests_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/artem/Documents/ASIO_PRACTICE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable tests_server"
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tests_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
FServer2_0/CMakeFiles/tests_server.dir/build: FServer2_0/tests_server

.PHONY : FServer2_0/CMakeFiles/tests_server.dir/build

FServer2_0/CMakeFiles/tests_server.dir/clean:
	cd /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 && $(CMAKE_COMMAND) -P CMakeFiles/tests_server.dir/cmake_clean.cmake
.PHONY : FServer2_0/CMakeFiles/tests_server.dir/clean

FServer2_0/CMakeFiles/tests_server.dir/depend:
	cd /home/artem/Documents/ASIO_PRACTICE/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/artem/Documents/ASIO_PRACTICE /home/artem/Documents/ASIO_PRACTICE/FServer2_0 /home/artem/Documents/ASIO_PRACTICE/build /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0 /home/artem/Documents/ASIO_PRACTICE/build/FServer2_0/CMakeFiles/tests_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : FServer2_0/CMakeFiles/tests_server.dir/depend
