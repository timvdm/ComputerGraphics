cmake_minimum_required(VERSION 2.8)
project(GfxGui)

find_package(Qt4)

# only build gui example if Qt4 is found
if (QT4_FOUND)
  message(STATUS "Qt4 found: building GUI example.")

  include_directories(../src)


  set(texture_HDRS
    gfxwidget.h
    texturewidget.h
  )

  set(texture_SRCS
    libgfx/
    texture.cpp
    gfxwidget.cpp
    texturewidget.cpp
  )

  qt4_wrap_cpp(texture_HDRS_MOC ${texture_HDRS})

  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

  add_executable(texture ${texture_SRCS} ${texture_HDRS_MOC})
  target_link_libraries(texture libgfx ${QT_LIBRARIES})

else()
  message(STATUS "Qt4 not found: not building GUI example.")
endif()
