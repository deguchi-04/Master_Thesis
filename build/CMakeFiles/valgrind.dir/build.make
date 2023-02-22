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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/thaidy/Documents/Master_Thesis

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/thaidy/Documents/Master_Thesis/build

# Utility rule file for valgrind.

# Include any custom commands dependencies for this target.
include CMakeFiles/valgrind.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/valgrind.dir/progress.make

CMakeFiles/valgrind:
	/usr/bin/valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes /home/thaidy/Documents/Master_Thesis/build/neural

valgrind: CMakeFiles/valgrind
valgrind: CMakeFiles/valgrind.dir/build.make
.PHONY : valgrind

# Rule to build all files generated by this target.
CMakeFiles/valgrind.dir/build: valgrind
.PHONY : CMakeFiles/valgrind.dir/build

CMakeFiles/valgrind.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/valgrind.dir/cmake_clean.cmake
.PHONY : CMakeFiles/valgrind.dir/clean

CMakeFiles/valgrind.dir/depend:
	cd /home/thaidy/Documents/Master_Thesis/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/thaidy/Documents/Master_Thesis /home/thaidy/Documents/Master_Thesis /home/thaidy/Documents/Master_Thesis/build /home/thaidy/Documents/Master_Thesis/build /home/thaidy/Documents/Master_Thesis/build/CMakeFiles/valgrind.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/valgrind.dir/depend

