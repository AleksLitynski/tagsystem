
cmake_minimum_required(VERSION 3.0.0)
project(tagsystem)

# Files and Directories
set(BIN ${CMAKE_BINARY_DIR})
set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(MDBDIR ${CMAKE_CURRENT_LIST_DIR}/lib/lmdb/libraries/liblmdb)
set(KLBDIR ${CMAKE_CURRENT_LIST_DIR}/lib/klib)
file(GLOB SRC_FILES ${SRC}/*.c ${SRC}/*.h)
file(GLOB KLB_H ${KLBDIR}/*.h)

# Set include paths
include_directories(${SRC})
include_directories(${MDBDIR})
include_directories(${KLBDIR})
link_directories(${BIN})

# Builds lmdb via make
add_custom_target(lib_lmdb COMMAND 
    cd ${MDBDIR} && 
    make && 
    cp liblmdb.a ${BIN} &&
    cp liblmdb.so ${BIN})

# builds the main library
SET(CMAKE_CXX_FLAGS "-g -std=c11 -D_GNU_SOURCE -Werror -Wfatal-errors -Wno-error=discarded-qualifiers -Wno-discarded-qualifiers")
add_library(ts STATIC ${SRC_FILES} ${KLB_H})
target_link_libraries(ts lmdb)
target_link_libraries(ts pthread)
add_dependencies(ts lib_lmdb)

# builds three test libraries
SET(CMAKE_CXX_FLAGS "-g -o2 -std=c11 -Werror -Wfatal-errors")
add_executable(test_main test/test.c)
target_link_libraries(test_main ts)

add_executable(test_redir test/redir.c)
target_link_libraries(test_redir ts)

add_executable(test_addremove test/addremove.c)
target_link_libraries(test_addremove ts)