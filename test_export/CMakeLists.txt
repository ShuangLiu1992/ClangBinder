cmake_minimum_required(VERSION 3.28)

project(TestExport CXX C)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        test_export.cc
        test_export.hh
)