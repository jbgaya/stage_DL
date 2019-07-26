# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_SOURCE_DIR = /home/jb/stage_DL/projects_pytorch/tutorials

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jb/stage_DL/projects_pytorch/tutorials/build

# Include any dependencies generated for this target.
include CMakeFiles/nnet_from_scratch.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/nnet_from_scratch.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/nnet_from_scratch.dir/flags.make

CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.o: CMakeFiles/nnet_from_scratch.dir/flags.make
CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.o: ../nnet_from_scratch.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jb/stage_DL/projects_pytorch/tutorials/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.o -c /home/jb/stage_DL/projects_pytorch/tutorials/nnet_from_scratch.cpp

CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jb/stage_DL/projects_pytorch/tutorials/nnet_from_scratch.cpp > CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.i

CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jb/stage_DL/projects_pytorch/tutorials/nnet_from_scratch.cpp -o CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.s

# Object files for target nnet_from_scratch
nnet_from_scratch_OBJECTS = \
"CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.o"

# External object files for target nnet_from_scratch
nnet_from_scratch_EXTERNAL_OBJECTS =

nnet_from_scratch: CMakeFiles/nnet_from_scratch.dir/nnet_from_scratch.cpp.o
nnet_from_scratch: CMakeFiles/nnet_from_scratch.dir/build.make
nnet_from_scratch: /usr/local/lib/libtorch.so
nnet_from_scratch: /usr/local/lib/libc10.so
nnet_from_scratch: /usr/lib/libcuda.so
nnet_from_scratch: /opt/cuda/lib/libnvrtc.so
nnet_from_scratch: /opt/cuda/lib64/libnvToolsExt.so
nnet_from_scratch: /opt/cuda/lib64/libcudart.so
nnet_from_scratch: /usr/local/lib/libc10_cuda.so
nnet_from_scratch: /usr/local/lib/libc10_cuda.so
nnet_from_scratch: /usr/local/lib/libcaffe2.so
nnet_from_scratch: /usr/local/lib/libc10.so
nnet_from_scratch: /opt/cuda/lib64/libcufft.so
nnet_from_scratch: /opt/cuda/lib64/libcurand.so
nnet_from_scratch: /usr/lib/libcudnn.so
nnet_from_scratch: /opt/cuda/lib64/libcublas.so
nnet_from_scratch: /opt/cuda/lib64/libcudart.so
nnet_from_scratch: CMakeFiles/nnet_from_scratch.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jb/stage_DL/projects_pytorch/tutorials/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable nnet_from_scratch"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nnet_from_scratch.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/nnet_from_scratch.dir/build: nnet_from_scratch

.PHONY : CMakeFiles/nnet_from_scratch.dir/build

CMakeFiles/nnet_from_scratch.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/nnet_from_scratch.dir/cmake_clean.cmake
.PHONY : CMakeFiles/nnet_from_scratch.dir/clean

CMakeFiles/nnet_from_scratch.dir/depend:
	cd /home/jb/stage_DL/projects_pytorch/tutorials/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jb/stage_DL/projects_pytorch/tutorials /home/jb/stage_DL/projects_pytorch/tutorials /home/jb/stage_DL/projects_pytorch/tutorials/build /home/jb/stage_DL/projects_pytorch/tutorials/build /home/jb/stage_DL/projects_pytorch/tutorials/build/CMakeFiles/nnet_from_scratch.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/nnet_from_scratch.dir/depend

