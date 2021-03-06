
OPTION(FORCE_DISABLE_CLANG "Set to ON if you don't want nextgen compilation" OFF)
OPTION(FORCE_RASPICAM "Set to ON if you are developer in CLion" OFF)

find_program(CLANG_PATH "clang")
find_program(CLANG_CXX_PATH "clang++")
IF(CLANG_PATH AND CLANG_CXX_PATH AND NOT FORCE_DISABLE_CLANG)
    message(STATUS "Clang comppiler found")
#    message(STATUS "CLANG_PATH = " ${CLANG_PATH})
#    message(STATUS "CLANG_CXX_PATH = " ${CLANG_CXX_PATH})
    set(CMAKE_C_COMPILER ${CLANG_PATH})
    set(CMAKE_CXX_COMPILER ${CLANG_CXX_PATH})
endif()

cmake_minimum_required(VERSION 3.0)
project(OpenCVClient)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CPLUS_INCLUDE_PATH /usr/include)

set(PROJECT_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/c++/headers)
set(PROJECT_NATIVE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/c++/native)
file(GLOB_RECURSE PROJECT_HEADERS_FILES ${PROJECT_HEADERS_DIR}/*.hpp)
file(GLOB_RECURSE PROJECT_NATIVE_FILES ${PROJECT_NATIVE_DIR}/*.cpp)

set(TLD_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/TLD/GeorgNebehay)
set(TLD_NATIVE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/TLD/GeorgNebehay)
file(GLOB_RECURSE TLD_HEADERS_FILES ${TLD_HEADERS_DIR}/*.h)
file(GLOB_RECURSE TLD_NATIVE_FILES ${TLD_NATIVE_DIR}/*.cpp)

set(CVBLOBS_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/cvblobs)
set(CVBLOBS_NATIVE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/cvblobs)
file(GLOB_RECURSE CVBLOBS_HEADERS_FILES ${CVBLOBS_HEADERS_DIR}/*.h)
file(GLOB_RECURSE CVBLOBS_NATIVE_FILES ${CVBLOBS_NATIVE_DIR}/*.cpp)

# build executable
include_directories(
        ${PROJECT_HEADERS_DIR}
        ${TLD_HEADERS_DIR}
        ${CVBLOBS_HEADERS_DIR}
)
add_executable(
        ${PROJECT_NAME}
        ${PROJECT_HEADERS_FILES} ${PROJECT_NATIVE_FILES}
        ${TLD_HEADERS_FILES} ${TLD_NATIVE_FILES}
        ${CVBLOBS_HEADERS_FILES} ${CVBLOBS_NATIVE_FILES}
)

# build and link subprojects
add_subdirectory(Properties)
add_subdirectory(OpenCVNetwork)
add_subdirectory(IntelEdison)
include_directories(${Properties_INCLUDE_DIRS})
include_directories(${OpenCVNetwork_INCLUDE_DIRS})
include_directories(${IntelEdison_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Properties)
target_link_libraries(${PROJECT_NAME} OpenCVNetwork)
target_link_libraries(${PROJECT_NAME} IntelEdison)

# link dependencies
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} pthread)
set(raspicam_DIR "/usr/local/lib/cmake")
find_package(raspicam)
IF(raspicam_CV_FOUND)
    target_link_libraries (${PROJECT_NAME} ${raspicam_CV_LIBS})
    add_definitions(-DUSE_RASPICAM=)
elseif(FORCE_RASPICAM)
    include_directories(raspicam.local/src)
    add_definitions(-DUSE_RASPICAM=)
#    ADD_SUBDIRECTORY(raspicam.local)
    MESSAGE( WARNING "Raspicam module force enabled.
    This might be used only for development not for release." )
else()
    MESSAGE( WARNING "Raspicam module usage will be unsupported in this build.
    Ensure you have installed Raspicam library and remake project.
    Raspicam module repository: https://github.com/cedricve/raspicam" )
endif()

#message( STATUS "** OpenCVNetwork_SOURCE_DIR: " ${OpenCVNetwork_SOURCE_DIR} )

#set optimization level
set(CMAKE_BUILD_TYPE Release)
