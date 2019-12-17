#Testing Utilities
option (BUILD_TESTING "Build Testing" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/testing)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/testing)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/testing)

if(BUILD_TESTING)
include(CTest)
enable_testing()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Rendering)

add_test("Runs" DecentImageViewer)
add_test("RenderTests" RenderTests)

endif()

# file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
# file(GLOB_RECURSE SRC_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# add_library(Test ${ExecutableFlags} ${SRC_FILES} ${SRC_CPP_FILES} ${HEADER_FILES})
# set_property(TARGET Test PROPERTY CXX_STANDARD 11)