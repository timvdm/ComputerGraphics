cmake_minimum_required(VERSION 2.8)
project(GfxGui)

set(programs
  axis
  texture
  light
  shadow
  pdb
)






find_package(Qt4)

# only build gui example if Qt4 is found
if (QT4_FOUND)
  message(STATUS "Qt4 found: building GUI example.")

  include_directories(../src)

  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

  foreach(prog ${programs})

    set(${prog}_HDRS
      gfxwidget.h
      ${prog}widget.h
    )

    set(${prog}_SRCS
      libgfx/
      gfxwidget.cpp
      ${prog}.cpp
      ${prog}widget.cpp
    )

    qt4_wrap_cpp(${prog}_HDRS_MOC ${${prog}_HDRS})

    add_executable(${prog} ${${prog}_SRCS} ${${prog}_HDRS_MOC})
    target_link_libraries(${prog} libgfx ${QT_LIBRARIES})

  endforeach()

else()
  message(STATUS "Qt4 not found: not building GUI example.")
endif()
