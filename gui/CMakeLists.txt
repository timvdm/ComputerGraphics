cmake_minimum_required(VERSION 2.8)
project(GfxGui)

include_directories(..)

set(programs
  texture
  light
  shadow
  fire
)


find_package(Qt4)

# only build gui example if Qt4 is found
if (QT4_FOUND)
  message(STATUS "Qt4 found: building GUI example.")


  include_directories(../src)

  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})
  
  add_subdirectory(pdbviewer)

  set(gfxwidget_HDRS gfxwidget.h)
  set(gfxwidget_SRCS gfxwidget.cpp)
  qt4_wrap_cpp(gfxwidget_HDRS_MOC ${gfxwidget_HDRS})
  add_library(gfxwidget STATIC ${gfxwidget_SRCS} ${gfxwidget_HDRS_MOC})

  foreach(prog ${programs})

    set(${prog}_HDRS
      ${prog}widget.h
    )

    set(${prog}_SRCS
      libgfx/
      ${prog}.cpp
      ${prog}widget.cpp
    )

    qt4_wrap_cpp(${prog}_HDRS_MOC ${${prog}_HDRS})

    add_executable(${prog} ${${prog}_SRCS} ${${prog}_HDRS_MOC})
    target_link_libraries(${prog} libgfx gfxwidget ${QT_LIBRARIES})

  endforeach()

else()
  message(STATUS "Qt4 not found: not building GUI example.")
endif()
