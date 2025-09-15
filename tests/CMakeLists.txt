cmake_minimum_required(VERSION 3.14)
project(websocket-test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  websocket-test
  crypto/base64.cpp
  crypto/sha1.cpp
  http/request.cpp
   ../src/http/http.c
   ../src/crypto/sha1.c
)

target_include_directories(
  websocket-test PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../src
)

target_link_libraries(
  websocket-test
  GTest::gtest_main
)

gtest_discover_tests(websocket-test)
