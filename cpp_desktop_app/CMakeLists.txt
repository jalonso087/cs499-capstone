cmake_minimum_required(VERSION 3.16)
project(TravlrAdmin VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.2/mingw_64" ${CMAKE_PREFIX_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

add_executable(TravlrAdmin
    main.cpp
    mainwindow.cpp
    mainwindow.h 
    bst.cpp
    bst.h
)

target_link_libraries(TravlrAdmin PRIVATE Qt6::Widgets Qt6::Network)