cmake_minimum_required (VERSION 3.1)
project (max_coverage)

set (CMAKE_CXX_STANDARD 11)

if(UNIX)
    add_compile_options("-march=native" "-Wall")
elseif(WIN32)
    add_compile_options("/Ot" "/W4")
endif()

add_executable(maxcover 
        ${PROJECT_SOURCE_DIR}/maxcover.cpp 
        ${PROJECT_SOURCE_DIR}/stopsignal.c
        ${PROJECT_SOURCE_DIR}/stopsignal.h
        ${PROJECT_SOURCE_DIR}/utils.h)

include_directories( ${PROJECT_SOURCE_DIR} )

if (UNIX)
    target_link_libraries(maxcover pthread)
endif()
