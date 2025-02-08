cmake_minimum_required(VERSION 3.13)
project(wsserver C)

# Set a default build type if not specified (for single-config generators)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Collect all .c files from the src directory and its subdirectories
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create the static library target
add_library(wsserver STATIC ${SRC_FILES})

# Set the output directory for the static library to the "lib" folder in the build directory
set_target_properties(wsserver PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add the src directory to the include path
target_include_directories(wsserver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Add compile options and definitions for each build type using generator expressions
target_compile_options(wsserver PRIVATE
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Release>:-O3>
  $<$<CONFIG:Release>:-fno-lto>
  $<$<CONFIG:Release>:-mtune=native>
  $<$<CONFIG:Release>:-ffast-math>
  $<$<CONFIG:Release>:-fno-math-errno>
  $<$<CONFIG:Release>:-falign-functions>
  $<$<CONFIG:Release>:-flto=auto>
)

target_compile_definitions(wsserver PRIVATE
  $<$<CONFIG:Debug>:LOG_LEVEL_DEBUG>
  $<$<CONFIG:Release>:LOG_LEVEL_ERROR>
)

# Add link options (applied to both Debug and Release configurations)
target_link_options(wsserver PRIVATE -flto)
