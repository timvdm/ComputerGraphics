include_directories(..)

set(cg_SRCS
  utils/EasyImage.cc
  utils/ini_configuration.cc
  utils/lparser.cc
  utils/vector.cc
  engine.cc
  labo/render.cpp
  labo/LineDrawing.cpp
  labo/LSystem2D.cpp
  labo/LSystem3D.cpp
  labo/Wireframe.cpp
  labo/ZBufferedWireframe.cpp
  labo/ZBuffering.cpp
)

add_executable(engine ${cg_SRCS})
target_link_libraries(engine libgfx)
