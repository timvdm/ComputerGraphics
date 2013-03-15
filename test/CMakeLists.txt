include_directories(../src)

enable_testing()

add_executable(test2d test2d.cpp)
target_link_libraries(test2d libgfx)
add_test(test2d_Test test/test2d)
