# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.22.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.22.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/siprave/Downloads/quickfix-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/siprave/Downloads/quickfix-master/build

# Include any dependencies generated for this target.
include src/CMakeFiles/pt.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/pt.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/pt.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/pt.dir/flags.make

src/CMakeFiles/pt.dir/pt.cpp.o: src/CMakeFiles/pt.dir/flags.make
src/CMakeFiles/pt.dir/pt.cpp.o: ../src/pt.cpp
src/CMakeFiles/pt.dir/pt.cpp.o: src/CMakeFiles/pt.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/siprave/Downloads/quickfix-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/pt.dir/pt.cpp.o"
	cd /Users/siprave/Downloads/quickfix-master/build/src && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/pt.dir/pt.cpp.o -MF CMakeFiles/pt.dir/pt.cpp.o.d -o CMakeFiles/pt.dir/pt.cpp.o -c /Users/siprave/Downloads/quickfix-master/src/pt.cpp

src/CMakeFiles/pt.dir/pt.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pt.dir/pt.cpp.i"
	cd /Users/siprave/Downloads/quickfix-master/build/src && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/siprave/Downloads/quickfix-master/src/pt.cpp > CMakeFiles/pt.dir/pt.cpp.i

src/CMakeFiles/pt.dir/pt.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pt.dir/pt.cpp.s"
	cd /Users/siprave/Downloads/quickfix-master/build/src && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/siprave/Downloads/quickfix-master/src/pt.cpp -o CMakeFiles/pt.dir/pt.cpp.s

src/CMakeFiles/pt.dir/getopt.c.o: src/CMakeFiles/pt.dir/flags.make
src/CMakeFiles/pt.dir/getopt.c.o: ../src/getopt.c
src/CMakeFiles/pt.dir/getopt.c.o: src/CMakeFiles/pt.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/siprave/Downloads/quickfix-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/CMakeFiles/pt.dir/getopt.c.o"
	cd /Users/siprave/Downloads/quickfix-master/build/src && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/CMakeFiles/pt.dir/getopt.c.o -MF CMakeFiles/pt.dir/getopt.c.o.d -o CMakeFiles/pt.dir/getopt.c.o -c /Users/siprave/Downloads/quickfix-master/src/getopt.c

src/CMakeFiles/pt.dir/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pt.dir/getopt.c.i"
	cd /Users/siprave/Downloads/quickfix-master/build/src && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/siprave/Downloads/quickfix-master/src/getopt.c > CMakeFiles/pt.dir/getopt.c.i

src/CMakeFiles/pt.dir/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pt.dir/getopt.c.s"
	cd /Users/siprave/Downloads/quickfix-master/build/src && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/siprave/Downloads/quickfix-master/src/getopt.c -o CMakeFiles/pt.dir/getopt.c.s

# Object files for target pt
pt_OBJECTS = \
"CMakeFiles/pt.dir/pt.cpp.o" \
"CMakeFiles/pt.dir/getopt.c.o"

# External object files for target pt
pt_EXTERNAL_OBJECTS =

src/pt: src/CMakeFiles/pt.dir/pt.cpp.o
src/pt: src/CMakeFiles/pt.dir/getopt.c.o
src/pt: src/CMakeFiles/pt.dir/build.make
src/pt: src/C++/libquickfix.16.0.1.dylib
src/pt: /usr/local/opt/openssl/lib/libssl.dylib
src/pt: /usr/local/opt/openssl/lib/libcrypto.dylib
src/pt: src/CMakeFiles/pt.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/siprave/Downloads/quickfix-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable pt"
	cd /Users/siprave/Downloads/quickfix-master/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pt.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/pt.dir/build: src/pt
.PHONY : src/CMakeFiles/pt.dir/build

src/CMakeFiles/pt.dir/clean:
	cd /Users/siprave/Downloads/quickfix-master/build/src && $(CMAKE_COMMAND) -P CMakeFiles/pt.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/pt.dir/clean

src/CMakeFiles/pt.dir/depend:
	cd /Users/siprave/Downloads/quickfix-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/siprave/Downloads/quickfix-master /Users/siprave/Downloads/quickfix-master/src /Users/siprave/Downloads/quickfix-master/build /Users/siprave/Downloads/quickfix-master/build/src /Users/siprave/Downloads/quickfix-master/build/src/CMakeFiles/pt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/pt.dir/depend

