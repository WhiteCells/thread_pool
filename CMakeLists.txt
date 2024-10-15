cmake_minimum_required(VERSION 3.5)

project(thread_pool_demo LANGUAGES CXX VERSION 1.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src thread_pool_lib_src)

# 创建共享库 (DLL)
add_library(thread_pool_lib SHARED ${thread_pool_lib_src})
set_target_properties(thread_pool_lib PROPERTIES OUTPUT_NAME "thread_pool")

aux_source_directory(${PROJECT_SOURCE_DIR}/ thread_pool_demo_src)

add_executable(${PROJECT_NAME} ${thread_pool_demo_src})

# 链接库
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE thread_pool_lib)
    # 在 Windows 上，确保 .dll 复制到可执行文件目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:thread_pool_lib>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
else()
    targettarget_link_libraries(${PROJECT_NAME} PRIVATE thread_pool_lib pthread)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})