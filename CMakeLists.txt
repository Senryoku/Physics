cmake_minimum_required(VERSION 3.1)
project(Physics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2")

INCLUDE_DIRECTORIES("src/")
set(SOURCE_FILES
    src/Constant.hpp
    src/Constraint.cpp
    src/Constraint.hpp
    src/Elastic.cpp
    src/Elastic.hpp
    src/Physics.hpp
    src/Polygon.cpp
    src/Polygon.hpp
    src/Rigid.cpp
    src/Rigid.hpp
    src/Vec2.hpp
    src/Vertex.cpp
    src/Vertex.hpp
    src/World.cpp
    src/World.hpp
)

set(TEST_SOURCE_FILES
    tests/FPSCounter.cpp
    tests/FPSCounter.hpp
    tests/Main.cpp
)

add_executable(Physics ${TEST_SOURCE_FILES} ${SOURCE_FILES})

#OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
#MESSAGE(STATUS "OpenGL Found ? ${OPENGL_FOUND}")
INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIR})
LINK_DIRECTORIES(${OpenGL_LIBRARY_DIRS})
ADD_DEFINITIONS(${OpenGL_DEFINITIONS})

#SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
set(SFML_ROOT "D:/Git/SFML/")
find_package(SFML 2 REQUIRED system window graphics)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(Physics ${SFML_LIBRARIES} ${OPENGL_LIBRARIES})
endif()
