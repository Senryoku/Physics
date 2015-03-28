cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0015 NEW)
cmake_policy(SET CMP0048 OLD)

project (Physics)
set (OpenGL_Toolbox_VERSION_MAJOR 0)
set (OpenGL_Toolbox_VERSION_MINOR 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXECUTABLE_NAME Exe)

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

MACRO(REC_SOURCE ParentDir)
	INCLUDE_DIRECTORIES(${ParentDir})
	AUX_SOURCE_DIRECTORY(${ParentDir} SOURCE_FILES)
	
	SUBDIRLIST(SUBDIRECTORIES ${ParentDir})

	foreach(DIR ${SUBDIRECTORIES})
		REC_SOURCE(${ParentDir}/${DIR})
		
	endforeach(DIR)
ENDMACRO()

REC_SOURCE(${CMAKE_SOURCE_DIR}/src)

list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -O3 -Wall -fopenmp ${CMAKE_CXX_FLAGS}")

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# Libraries ------------------------------------------------------------------------

#OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
#MESSAGE(STATUS "OpenGL Found ? ${OPENGL_FOUND}")
INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIR})
LINK_DIRECTORIES(${OpenGL_LIBRARY_DIRS})
ADD_DEFINITIONS(${OpenGL_DEFINITIONS})

#SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${OPENGL_LIBRARIES})
endif()

# -----------------------------------------------------------------------------------

add_custom_target(run
    COMMAND bin/${EXECUTABLE_NAME}.exe
    DEPENDS ${EXECUTABLE_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

