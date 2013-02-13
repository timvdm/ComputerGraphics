cmake_minimum_required(VERSION 2.8)
project(ComputerGraphics)

set(cg_SRCS
  src/utils/EasyImage.cc
  src/utils/ini_configuration.cc
  src/engine.cc
  src/week1/ColorRectangle.cc
  src/week1/Blocks.cc
)

add_executable(engine ${cg_SRCS})
