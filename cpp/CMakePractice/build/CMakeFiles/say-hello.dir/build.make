# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build

# Include any dependencies generated for this target.
include CMakeFiles/say-hello.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/say-hello.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/say-hello.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/say-hello.dir/flags.make

CMakeFiles/say-hello.dir/hello.cpp.o: CMakeFiles/say-hello.dir/flags.make
CMakeFiles/say-hello.dir/hello.cpp.o: ../hello.cpp
CMakeFiles/say-hello.dir/hello.cpp.o: CMakeFiles/say-hello.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/say-hello.dir/hello.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/say-hello.dir/hello.cpp.o -MF CMakeFiles/say-hello.dir/hello.cpp.o.d -o CMakeFiles/say-hello.dir/hello.cpp.o -c /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/hello.cpp

CMakeFiles/say-hello.dir/hello.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/say-hello.dir/hello.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/hello.cpp > CMakeFiles/say-hello.dir/hello.cpp.i

CMakeFiles/say-hello.dir/hello.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/say-hello.dir/hello.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/hello.cpp -o CMakeFiles/say-hello.dir/hello.cpp.s

# Object files for target say-hello
say__hello_OBJECTS = \
"CMakeFiles/say-hello.dir/hello.cpp.o"

# External object files for target say-hello
say__hello_EXTERNAL_OBJECTS =

libsay-hello.dylib: CMakeFiles/say-hello.dir/hello.cpp.o
libsay-hello.dylib: CMakeFiles/say-hello.dir/build.make
libsay-hello.dylib: CMakeFiles/say-hello.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libsay-hello.dylib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/say-hello.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/say-hello.dir/build: libsay-hello.dylib
.PHONY : CMakeFiles/say-hello.dir/build

CMakeFiles/say-hello.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/say-hello.dir/cmake_clean.cmake
.PHONY : CMakeFiles/say-hello.dir/clean

CMakeFiles/say-hello.dir/depend:
	cd /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build /Users/David/Documents/Learning_and_tutorials/cpp/CMakePractice/build/CMakeFiles/say-hello.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/say-hello.dir/depend
