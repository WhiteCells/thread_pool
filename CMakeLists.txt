cmake_minimum_required(VERSION 3.5)

project(thread_pool_demo LANGUAGES CXX VERSION 1.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(thread_pool_lib_name thread_pool_lib)

aux_source_directory(${PROJECT_SOURCE_DIR}/src thread_pool_lib_src)

add_library(${thread_pool_lib_name} SHARED ${thread_pool_lib_src})

aux_source_directory(${PROJECT_SOURCE_DIR}/ thread_pool_demo_src)

add_executable(${PROJECT_NAME} ${thread_pool_demo_src})

target_link_libraries(${PROJECT_NAME} PRIVATE ${thread_pool_lib_name} pthread)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})