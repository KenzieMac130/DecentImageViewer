file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(RenderTests ${ExecutableFlags} ${SRC_FILES} ${HEADER_FILES})
set_property(TARGET RenderTests PROPERTY C_STANDARD 99)
set_property(TARGET RenderTests PROPERTY FOLDER "Testing")

target_link_libraries(RenderTests ${SDL2_LIBRARIES})
target_link_libraries(RenderTests Renderer)

if(RENDERER STREQUAL "OpenGL")
	target_link_libraries(RenderTests ${OPENGL_LIBRARIES})
	target_link_libraries(RenderTests ${GLEW_LIBRARIES})
endif()