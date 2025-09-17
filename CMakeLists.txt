cmake_minimum_required(VERSION 3.13)
project(wsserver C)
enable_language(ASM)

# Set a default build type if not specified (for single-config generators)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

if (APPLE)
  file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
  set(DEBUG_COMPILE_OPTIONS "-O0" "-g")
  set(RELEASE_COMPILE_OPTIONS "-O3" "-mtune=native" "-ffast-math" "-fno-math-errno" "-falign-functions")
else()
  file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
  file(GLOB_RECURSE ASM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.S")
  set(DEBUG_COMPILE_OPTIONS "-O0" "-static-libasan" "-fno-builtin" "-g")
  set(RELEASE_COMPILE_OPTIONS "-O3" "-mtune=native" "-ffast-math" "-fno-math-errno" "-fno-builtin" "-falign-functions")
endif()

# Create the static library target including C and assembly files
add_library(wsserver STATIC ${SRC_FILES} ${ASM_FILES})
add_library(wsserver_shared SHARED ${SRC_FILES} ${ASM_FILES})

# Set the output directory for the static library to the "lib" folder in the build directory
set_target_properties(wsserver PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add the src directory to the include path
target_include_directories(wsserver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_options(
  wsserver PRIVATE
  $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
  $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)

target_compile_definitions(
  wsserver PRIVATE
  $<$<CONFIG:Debug>:LOG_LEVEL_DEBUG>
  $<$<CONFIG:Release>:LOG_LEVEL_ERROR>
)

# Add link options (applied to both Debug and Release configurations)
#target_link_options(wsserver PRIVATE -flto)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(BUILD_SHARED_LIBS OFF)

install(TARGETS wsserver
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/websocket/websocket.h"
        DESTINATION include)

