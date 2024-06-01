cmake_minimum_required(VERSION 3.22...3.25)

project(
        NativeInstrumentation
        VERSION 1.0
        LANGUAGES CXX
)
find_package(JNI)


if (NOT JNI_FOUND)
    message(FATAL_ERROR "Could not find JNI")
else ()
    message(STATUS "Found JNI directory ${JNI_INCLUDE_DIRS}")
endif ()

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with CMakeLists.txt file). "
            "Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

add_subdirectory(native-instrumentation-native)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
