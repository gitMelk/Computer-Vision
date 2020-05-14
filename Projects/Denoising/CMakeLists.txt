cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(HM3_CV)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_library(filter
	filter.h	
	filter.cpp)

add_executable(${PROJECT_NAME} HM3_CV.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})