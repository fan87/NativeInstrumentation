cmake_minimum_required(VERSION 3.22...3.25)


add_library(native_instrumentation_native SHARED
        type_convert.h
        javah/me_fan87_nativeinstrumentation_NativeInstrumentation.h
)


target_sources(native_instrumentation_native PRIVATE
        lib.cpp
        type_convert.cpp
)
target_include_directories(native_instrumentation_native
        PRIVATE ${JNI_INCLUDE_DIRS}
)
if (${MEMCHECK})
    target_compile_definitions(native_instrumentation_native PRIVATE MEMCHECK)
endif ()

target_link_libraries(native_instrumentation_native ${JAVA_JVM_LIBRARY})

add_custom_command(TARGET native_instrumentation_native POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
        "$<TARGET_FILE:native_instrumentation_native>"
        "${CMAKE_BINARY_DIR}/lib/$<TARGET_FILE_NAME:native_instrumentation_native>"
        COMMENT "Copying to output directory")

set_target_properties(native_instrumentation_native PROPERTIES LINK_FLAGS_RELEASE -s)