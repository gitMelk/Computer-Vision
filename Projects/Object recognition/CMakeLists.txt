cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(HM6_CV)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_library(ObjDect
	src/ObjDect.h 
	src/ObjDect.cpp
	)

add_executable(HM6_CV src/HM6_CV.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ObjDect)
