cmake_minimum_required(VERSION 2.8)
file(GLOB tritest_SRC "*.cpp")
add_executable(tritest ${tritest_SRC})
target_link_libraries(tritest -lSDL2 -lGLEW -lGL)