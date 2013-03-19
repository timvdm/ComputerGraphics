cmake_minimum_required(VERSION 2.8)
project(ComputerGraphics)

# setup Eigen3 includes
include_directories(thirdparty/eigen3)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-Wno-enum-compare -std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

set(libgfx_SRCS
  src/libgfx/transform.cpp
  src/libgfx/mesh.cpp
)

add_library(libgfx SHARED ${libgfx_SRCS})

set(cg_SRCS
  src/utils/EasyImage.cc
  src/utils/ini_configuration.cc
  src/utils/lparser.cc
  src/utils/vector.cc
  src/engine.cc
  src/labo/render.cpp
  src/labo/LineDrawing.cpp
  src/labo/LSystem2D.cpp
  src/labo/LSystem3D.cpp
  src/labo/Wireframe.cpp
)

add_executable(engine ${cg_SRCS})
target_link_libraries(engine libgfx)

add_subdirectory(gui)

#add_subdirectory(test)
#add_subdirectory(OpenGL)


##################################################
#
# CPack
#
##################################################

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ComputerGraphics")
set(CPACK_PACKAGE_VENDOR "Tim Vandermeersch")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

set(ignore_files
  .git
  .gitignore
  build
  test
  libxy
  opgaves
  OpenGL
  gen_html.py
  run_engine.py
)

set(CPACK_SOURCE_IGNORE_FILES "${ignore_files}")

include(CPack)
