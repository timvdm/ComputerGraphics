cmake_minimum_required(VERSION 2.8)
project(ComputerGraphics)

# setup Eigen3 includes
include_directories(thirdparty/eigen3)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-Wall -Wno-reorder -Wno-sign-compare -Wno-enum-compare -std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(src)
add_subdirectory(libgfx)
add_subdirectory(gui)

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
  data/house.svg
)

set(CPACK_SOURCE_IGNORE_FILES "${ignore_files}")

include(CPack)
