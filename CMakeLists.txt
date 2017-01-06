
cmake_minimum_required(VERSION 3.0.0)
project(tagsystem)



set(BIN ${CMAKE_BINARY_DIR})
set(SRC ${CMAKE_CURRENT_LIST_FILE}/src)
set(MDBDIR ${CMAKE_CURRENT_LIST_FILE}/lib/lmdb/libraries/liblmdb)
set(KLBDIR ${MY_SOURCE_DIR}/lib/klib)

include_directories(${SRC})
include_directories(${MDBDIR})
include_directories(${KLBDIR})

find_package(lmdb)
find_package(pthread)

# builds lmdb via make
add_custom_target(lmdb COMMAND cd ${MDBDIR} && make && cp liblmdb.a ${bin}/liblmdb.a)

# builds the main library
SET(CMAKE_CXX_FLAGS "-g -std=c11 -D_GNU_SOURCE -Werror -Wfatal-errors -Wno-error=discarded-qualifiers -Wno-discarded-qualifiers")
set(SOURCE_FILES ts.c tsdoc.c tsenv.c tsiter.c tsnode.c tssearch.c tstag.c tsutil.c tswalk.c)
add_library(libts STATIC ${SOURCE_FILES})
target_link_libraries(libts ${lmdb};${pthread})
find_package(libts)


# builds three test libraries
SET(CMAKE_CXX_FLAGS "-g -o2 -std=c11 -Werror -Wfatal-errors")
add_executable(test_main test/test.c)
target_link_libraries(test_main ${libts};${pthread};${lmdb})

add_executable(test_redir test/redir.c)
target_link_libraries(test_redir ${libts})

add_executable(test_addremove test/addremove.c)
target_link_libraries(test_addremove ${libts};${lmdb})
