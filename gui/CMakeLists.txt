cmake_minimum_required(VERSION 2.8)
project(GfxGui)

find_package(Qt4)

# only build gui example if Qt4 is found
if (QT4_FOUND)
  message(STATUS "Qt4 found: building GUI example.")

  include_directories(../src)


  set(gfxgui_HDRS
    gfxwidget.h
  )

  set(gfxgui_SRCS
    main.cpp
    gfxwidget.cpp
  )

  qt4_wrap_cpp(gfxgui_HDRS_MOC ${gfxgui_HDRS})

  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

  add_executable(gui ${gfxgui_SRCS} ${gfxgui_HDRS_MOC})
  target_link_libraries(gui libgfx ${QT_LIBRARIES})

else()
  message(STATUS "Qt4 not found: not building GUI example.")
endif()
