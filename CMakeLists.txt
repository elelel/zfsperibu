cmake_minimum_required(VERSION 3.4.0)
project(zfsperibu)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")

set(SRC "src")

include_directories(
  ${SRC}
  )

set(SOURCES
  ${SRC}/timestamp.cpp
  ${SRC}/snapshot.cpp
  ${SRC}/zfsperibu.cpp)

add_executable(zfsperibu ${SOURCES})
