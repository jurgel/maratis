project(GLEW)
cmake_minimum_required(VERSION 2.8.12)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(GLEW_VERSION "1.11.0")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CFLAGS} ${CMAKE_C_FLAGS} -DGLEW_BUILD -DGLEW_NO_GLU -O2 -Wall -W" )

include_directories( ${PROJECT_SOURCE_DIR}/include )

add_library(GLEW_static STATIC src/glew.c )
add_library(GLEW_MX_static STATIC src/glew.c )
