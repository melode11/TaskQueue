cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(TaskQueue)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR})
add_library(taskqueue STATIC ${ALL_SRC} )

