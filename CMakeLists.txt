cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(ofv-bga)

# find_package(PCL 1.2 REQUIRED)

include_directories(
  include
  ../tiny_instruments/include  
)

# link_directories(${PCL_LIBRARY_DIRS})

# add_definitions(${PCL_DEFINITIONS})

add_library(bga_prep SHARED
  src/individ.cpp
  src/task_parameters.cpp
  src/evo_pipe.cpp
  src/st_solver_for_BGA.cpp
  src/task_ios.cpp
  src/norm_interface.cpp
)

# add_executable(st_for_bga_test
#   src/tests/st_solver_test.cpp
# )
# target_link_libraries (st_for_bga_test bga)


if(DEBUG_OUTPUT)
  set_target_properties(bga PROPERTIES COMPILE_DEFINITIONS "DEBUG_OUTPUT")
endif()