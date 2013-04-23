  
include_directories(${CMAKE_BINARY_DIR}/gui/pdbviewer)
include_directories(${CMAKE_SOURCE_DIR}/gui/pdbviewer)

set(pdbviewer_HDRS
  mainwindow.h
  gfxwidget.h
  pdbwidget.h
)

set(pdbviewer_SRCS
  main.cpp
  mainwindow.cpp
  gfxwidget.cpp
  pdbwidget.cpp
)

set(pdbviewer_FORMS
  mainwindow.ui
)

qt4_wrap_cpp(pdbviewer_HDRS_MOC ${pdbviewer_HDRS})
qt4_wrap_ui(pdbviewer_FORM_HDRS ${pdbviewer_FORMS})

add_executable(pdbviewer ${pdbviewer_SRCS} ${pdbviewer_HDRS_MOC} ${pdbviewer_FORM_HDRS})
target_link_libraries(pdbviewer libgfx gfxwidget ${QT_LIBRARIES})

