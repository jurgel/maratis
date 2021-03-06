## Find source and headers
FILE(GLOB HEADERS
  *.h
  ${MARATIS_COMMON_INCLUDE_DIR}/GUI/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Contexts/Input/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Contexts/Bullet/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Loaders/FreeImage/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Loaders/Freetype/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Loaders/Mesh/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Renderers/Fixed/*.h
  ${MARATIS_COMMON_INCLUDE_DIR}/Renderers/Standard/*.h
)
FILE(GLOB SOURCES
  *.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/GUI/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Contexts/Input/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Contexts/Bullet/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Loaders/FreeImage/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Loaders/Freetype/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Loaders/Mesh/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Renderers/Fixed/*.c*
  ${MARATIS_COMMON_INCLUDE_DIR}/Renderers/Standard/*.c*
)


IF(ANDROID)
	FILE(GLOB OS_HEADERS
	${MARATIS_COMMON_INCLUDE_DIR}/Contexts/ES2/*.h
	)
	FILE(GLOB OS_SOURCES
	${MARATIS_SOURCE_DIR}/3rdparty/SDL2/src/main/android/SDL_android_main.c
	${MARATIS_COMMON_INCLUDE_DIR}/Contexts/ES2/*.c*
	)
ELSE()
	FILE(GLOB OS_HEADERS
	${MARATIS_COMMON_INCLUDE_DIR}/Contexts/GL/*.h
	)
	FILE(GLOB OS_SOURCES
	${MARATIS_COMMON_INCLUDE_DIR}/Contexts/GL/*.c*
	)
ENDIF()

IF(CMAKE_HAS_SORT)
  LIST(SORT HEADERS)
  LIST(SORT SOURCES)
  LIST(SORT OS_HEADERS)
  LIST(SORT OS_SOURCES)
ENDIF(CMAKE_HAS_SORT)

## Build
INCLUDE_DIRECTORIES(
  ## Maratis include directories
  ${MARATIS_COMMON_INCLUDE_DIR}
  ${MCORE_INCLUDE_DIR}
  ${MENGINE_INCLUDE_DIR}
  ${MGUI_INCLUDE_DIR}

  ## 3rdparty include directories
  ${FREETYPE_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${TINYXML_INCLUDE_DIR}
  ${FREEIMAGE_INCLUDE_DIR}
  ${SDL_INCLUDE_DIR}
  ${TINYCTHREAD_INCLUDE_DIR}
  ${TINYUTF8_INCLUDE_DIR}
  ${BULLET_INCLUDE_DIR}
)

IF(ANDROID)
	ADD_LIBRARY(ManualUse SHARED ${SOURCES} ${HEADERS} ${OS_SOURCES} ${OS_HEADERS})
ELSE()
	ADD_EXECUTABLE(ManualUse ${SOURCES} ${HEADERS} ${OS_SOURCES} ${OS_HEADERS})
ENDIF()

ADD_DEFINITIONS(-DFREEIMAGE_LIB)

TARGET_LINK_LIBRARIES(ManualUse
  ## Maratis libraries
  ${MCORE_LIBRARIES}
  ${MENGINE_LIBRARIES}
  ${MGUI_LIBRARIES}

  ## 3rdparty libraries
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${FREEIMAGE_LIBRARIES}
  ${SDL_LIBRARIES}
  ${TINYCTHREAD_LIBRARIES}
  ${BULLET_LIBRARIES}

  ## System libraries
  ${OPENGL_LIBRARIES}
  ${PLATFORM_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
)

## Install
INSTALL(TARGETS ManualUse DESTINATION ${MARATIS_INSTALL_DIR})
INSTALL(DIRECTORY data DESTINATION ${MARATIS_INSTALL_DIR})

IF(APPLE)
  SET_TARGET_PROPERTIES(
	ManualUse
	PROPERTIES
	BUILD_WITH_INSTALL_RPATH 1
	INSTALL_RPATH "@loader_path/"
  )
ENDIF(APPLE)

IF(UNIX AND NOT APPLE)
  SET_TARGET_PROPERTIES(
	ManualUse
	PROPERTIES
	BUILD_WITH_INSTALL_RPATH 1
	INSTALL_RPATH "$ORIGIN/"
  )
ENDIF(UNIX AND NOT APPLE)
