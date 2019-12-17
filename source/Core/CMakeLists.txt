file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

if(WIN32)
set(ExecutableFlags WIN32)
endif()

add_executable(DecentImageViewer ${ExecutableFlags} ${SRC_FILES} ${HEADER_FILES})
set_property(TARGET DecentImageViewer PROPERTY C_STANDARD 99)
set_property(TARGET DecentImageViewer PROPERTY FOLDER "DecentImageViewer")

target_link_libraries(DecentImageViewer ${SDL2_LIBRARIES})
target_link_libraries(DecentImageViewer Renderer)

if(RENDERER STREQUAL "OpenGL")
	target_link_libraries(DecentImageViewer ${OPENGL_LIBRARIES})
	target_link_libraries(DecentImageViewer ${GLEW_LIBRARIES})
endif()