cmake_minimum_required(VERSION 2.6)

project(PHILIPS-TO-ISMRMRD)

#VERSIONING
set(PHILIPS_TO_ISMRMRD_VERSION_MAJOR 0)
set(PHILIPS_TO_ISMRMRD_VERSION_MINOR 0)
set(PHILIPS_TO_ISMRMRD_VERSION_PATCH 1)
set(PHILIPS_TO_ISMRMRD_VERSION_STRING ${PHILIPS_TO_ISMRMRD_VERSION_MAJOR}.${PHILIPS_TO_ISMRMRD_VERSION_MINOR}.${PHILIPS_TO_ISMRMRD_VERSION_PATCH})

if (WIN32)
    ADD_DEFINITIONS(-DWIN32 -D_WIN32 -D_WINDOWS)
    # ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    SET (CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    SET (CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    SET (CMAKE_STATIC_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    SET (CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")

    #ADD_DEFINITIONS(-D__BUILD_CONVERTER_TINYXML__)
else (WIN32)
  if (UNIX)
    if (APPLE)
      SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else (APPLE)
      SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11")
    endif (APPLE)
  endif (UNIX)
endif (WIN32)

###############################################################
find_path(ISMRMRD_CMAKE_MODULES FindIsmrmrd.cmake HINTS
  $ENV{ISMRMRD_HOME}/share/ismrmrd/cmake
  ${CMAKE_INSTALL_PREFIX}/share/ismrmrd/cmake
  /usr/local/share/ismrmrd/cmake)

if (NOT ISMRMRD_CMAKE_MODULES)
   #TODO: Check if path found otherwise create error
   message(FATAL_ERROR "ISMRMRD_CMAKE_MODULES cannot be found. Try to set ISMRMRD_HOME environment variable.")
else (NOT ISMRMRD_CMAKE_MODULES)
   message("ISMRMRD_CMAKE_MODULES found.")
endif(NOT ISMRMRD_CMAKE_MODULES)

set(CMAKE_MODULE_PATH ${ISMRMRD_CMAKE_MODULES})
###############################################################

# Added for static linking!!!
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread system program_options filesystem REQUIRED)

find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR})

find_package(Ismrmrd REQUIRED)
INCLUDE_DIRECTORIES( ${ISMRMRD_INCLUDE_DIR} )

add_executable(philips_to_ismrmrd 
               main.cpp
	       pugixml.cpp)

target_link_libraries(philips_to_ismrmrd ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES} ${ISMRMRD_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS philips_to_ismrmrd DESTINATION bin)
