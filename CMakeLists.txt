cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(ofv-bga)

# find_package(PCL 1.2 REQUIRED)

include_directories(
  include
  ../tiny_instruments/include  
)

# link_directories(${PCL_LIBRARY_DIRS})

# add_definitions(${PCL_DEFINITIONS})

add_library(bga SHARED
  src/task.cpp
  src/individ.cpp
  src/task_parameters.cpp
  src/evo_pipe.cpp
  src/st_solver_for_BGA
  src/task_ios.cpp
  src/norm_interface.cpp
)

# target_link_libraries (greedy_projection ${PCL_LIBRARIES})