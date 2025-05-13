cmake_minimum_required(VERSION 3.10)
project(Lab8)

find_package(OpenCV REQUIRED)

add_executable(main code/src/main.cpp
)


target_link_libraries(main ${OpenCV_LIBS})

