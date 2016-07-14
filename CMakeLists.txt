CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(GeometryModel)

# The version number.
set (GEOMETRYMODEL_VERSION_MAJOR 0)
set (GEOMETRYMODEL_VERSION_MINOR 1)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_language(CXX)

set (GEOMETRYMODEL_LIBRARIES)

find_package(OpenCasCade)
IF(NOT OCC_FOUND)
  message(SEND_ERROR, "Cannot find OpenCasCade - required.")
ELSE()
  include_directories(${OCC_INCLUDE_DIR})
  set (GEOMETRYMODEL_LIBRARIES ${OCC_LIBRARIES} ${GEOMETRYMODEL_LIBRARIES})
ENDIF(NOT OCC_FOUND)

FIND_PACKAGE(VTK NO_MODULE)
if(VTK_FOUND)
  message(STATUS "Found VTK: ${VTK_DIR} (found version \"${VTK_VERSION}\")")

  include(${VTK_USE_FILE})
  set (GEOMETRYMODEL_LIBRARIES ${VTK_LIBRARIES} ${GEOMETRYMODEL_LIBRARIES})
  add_definitions(-DVTK_FOUND)
else()
  message(SEND_ERROR, "Cannot find VTK - required.")
endif()
 
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-literal-suffix -Wno-deprecated")
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  message(STATUS "Adding magic sauce for Intel compiler.")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -xHOST -g")
endif()

include_directories(include ${CMAKE_BINARY_DIR}/include)

file(GLOB CXX_SOURCES src/*.cpp)

# target_link_library
add_library(geometrymodel SHARED ${CXX_SOURCES})
target_link_libraries(geometrymodel ${GEOMETRYMODEL_LIBRARIES})

install(FILES libgeometrymodel.so DESTINATION lib)

# Tests
include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

ADD_EXECUTABLE(test_GeometryModel tests/test_GeometryModel.cpp)
TARGET_LINK_LIBRARIES(test_GeometryModel ${GEOMETRYMODEL_LIBRARIES} geometrymodel)

ADD_EXECUTABLE(test_CAD_Formats tests/test_CAD_Formats.cpp)
TARGET_LINK_LIBRARIES(test_CAD_Formats ${GEOMETRYMODEL_LIBRARIES} geometrymodel)

add_test(NAME unit_GeometryModel COMMAND ${PROJECT_BINARY_DIR}/tests/test_GeometryModel)
add_test(NAME unit_CAD_Format COMMAND ${PROJECT_BINARY_DIR}/tests/test_CAD_Formats)
