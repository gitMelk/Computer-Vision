cmake_minimum_required(VERSION 3.2)                         
PROJECT(HW2)                                    

FIND_PACKAGE( OpenCV REQUIRED )                             
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

ADD_EXECUTABLE(HW2 HM2_CV.cpp)                         
TARGET_LINK_LIBRARIES (HW2 ${OpenCV_LIBS})        
