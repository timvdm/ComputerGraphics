

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

add_executable(cube cube.cpp)
target_link_libraries(cube ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
