
cmake_minimum_required(VERSION 2.8)
project(LibGFX)

# setup Eigen3 includes
include_directories(thirdparty/eigen3)

set(libgfx_SRCS
  EasyImage.cc
  transform.cpp
  mesh.cpp
  texture.cpp
)

add_library(libgfx SHARED ${libgfx_SRCS})
