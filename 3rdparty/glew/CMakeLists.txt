project(GLEW)
cmake_minimum_required(VERSION 2.8.12)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(GLEW_VERSION "1.11.0")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
	set(GLEW_LIB_NAME glew32)
else(WIN32)
	set(GLEW_LIB_NAME GLEW)
	set(DLL_PREFIX lib)
endif(WIN32)

#
# All platforms need OpenGL
#

set(OPENGL_LIB "")
if(WIN32)
	list( APPEND MLIBS opengl32 )
	list( APPEND MLIBS gdi32 )
	list( APPEND MLIBS user32 )
	list( APPEND MLIBS kernel32 )
elseif(ANDROID)
	set(OPENGL_LIB GLESv2)
else()
	include(FindPkgConfig)
	pkg_check_modules( OpenGL REQUIRED gl )
	find_library(OPENGL_LIB OpenGL)
endif()

#
# Linux needs X11
#

IF(UNIX AND NOT APPLE AND NOT ANDROID)
	find_package(X11 REQUIRED)
ENDIF(UNIX AND NOT APPLE)

set(CMAKE_C_FLAGS "${CFLAGS} ${CMAKE_C_FLAGS} -DGLEW_BUILD -DGLEW_NO_GLU -O2 -Wall -W" )

include_directories( ${PROJECT_SOURCE_DIR}/include )

add_library(GLEW_static STATIC src/glew.c )
add_library(GLEW_shared SHARED src/glew.c )
set_target_properties(GLEW_static PROPERTIES OUTPUT_NAME ${GLEW_LIB_NAME} PREFIX lib)
set_target_properties(GLEW_shared PROPERTIES OUTPUT_NAME ${GLEW_LIB_NAME} PREFIX "${DLL_PREFIX}")
target_link_libraries(GLEW_shared ${OPENGL_LIB})

add_library(GLEW_MX_static STATIC src/glew.c )
add_library(GLEW_MX_shared SHARED src/glew.c )
set_target_properties(GLEW_MX_static PROPERTIES OUTPUT_NAME ${GLEW_LIB_NAME}mx COMPILE_FLAGS "-DGLEW_MX" PREFIX lib)
set_target_properties(GLEW_MX_shared PROPERTIES OUTPUT_NAME ${GLEW_LIB_NAME}mx COMPILE_FLAGS "-DGLEW_MX" PREFIX "${DLL_PREFIX}")
target_link_libraries(GLEW_MX_shared ${OPENGL_LIB})

