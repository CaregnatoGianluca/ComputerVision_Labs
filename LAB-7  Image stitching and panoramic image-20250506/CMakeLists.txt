cmake_minimum_required(VERSION 3.10)
project(Lab7)

find_package(OpenCV REQUIRED)

add_executable(main code/src/main.cpp 
    code/src/panoramic_utils.cpp
    code/src/stitcher.cpp)


target_link_libraries(main ${OpenCV_LIBS})

