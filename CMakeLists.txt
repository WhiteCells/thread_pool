cmake_minimum_required(VERSION 3.5)

project(thread_pool LANGUAGES CXX VERSION 1.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(${PROJECT_SOURCE_DIR}/ src)

add_executable(${PROJECT_NAME} ${src})