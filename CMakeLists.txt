cmake_minimum_required(VERSION 2.8)
project(ComputerGraphics)

#set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

set(cg_SRCS
  src/utils/EasyImage.cc
  src/utils/ini_configuration.cc
  src/utils/lparser.cc
  src/utils/vector.cc
  src/engine.cc
  src/lines2d.cc
  src/week1/ColorRectangle.cc
  src/week1/Blocks.cc
  src/week1/Lines.cc
  src/week2/LineDrawing.cc
  src/week2/LSystem2D.cc
  src/week3/Wireframe.cc
)

add_executable(engine ${cg_SRCS})

set(libgfx_SRCS
  src/utils/EasyImage.cc
  #  src/utils/vector.cc
  src/libgfx/utility.cpp
)

add_library(libgfx SHARED ${libgfx_SRCS})

add_executable(gfx src/gfx.cpp)
target_link_libraries(gfx libgfx)

add_subdirectory(gui)
