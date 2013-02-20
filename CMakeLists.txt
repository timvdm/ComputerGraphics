cmake_minimum_required(VERSION 2.8)
project(ComputerGraphics)

set(cg_SRCS
  src/utils/EasyImage.cc
  src/utils/ini_configuration.cc
  src/utils/lparser.cc
  src/engine.cc
  src/week1/ColorRectangle.cc
  src/week1/Blocks.cc
  src/week1/Lines.cc
  src/week2/lines2d.cc
  src/week2/LineDrawing.cc
  src/week2/LSystem2D.cc
)

add_executable(engine ${cg_SRCS})
