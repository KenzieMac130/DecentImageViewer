#Add Shaders
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/InternShaders)

#Shader Code Generator
if(RENDERER STREQUAL "OpenGL")
	file(GLOB_RECURSE SHADER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/InternShaders/GLSL/*.glsl)
	set(SHADER_GEN_OUTPUT_H "${CMAKE_CURRENT_BINARY_DIR}/CodeGen/Shaders/GL/gen_ShaderImplementation.h")
	set(SHADER_GEN_OUTPUT_C "${CMAKE_CURRENT_BINARY_DIR}/CodeGen/Shaders/GL/gen_ShaderImplementation.c")
	add_custom_command(
		OUTPUT ${SHADER_GEN_OUTPUT_C} ${SHADER_GEN_OUTPUT_H}
		COMMAND ShaderImplCodeGenerator GLSL ${SHADER_GEN_OUTPUT_C} ${SHADER_GEN_OUTPUT_H} ${CMAKE_CURRENT_SOURCE_DIR}/InternShaders/GLSL/
		DEPENDS SHADER_SRC_FILES
	)
endif()

#Renderer
if(RENDERER STREQUAL "OpenGL")
	file(GLOB_RECURSE RNDR_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/GL/*.c)
	file(GLOB_RECURSE RNDR_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/GL/*.h)
endif()

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(Renderer 
${RNDR_SRC_FILES} ${RNDR_HEADER_FILES}
${SRC_FILES} ${HEADER_FILES}
${SHADER_SRC_FILES}
${SHADER_GEN_OUTPUT_C} ${SHADER_GEN_OUTPUT_H})
set_property(TARGET Renderer PROPERTY C_STANDARD 99)
set_property(TARGET Renderer PROPERTY FOLDER "DecentImageViewer/Components")

if(RENDERER STREQUAL "OpenGL")
target_include_directories(Renderer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(Renderer ${SDL2_LIBRARIES})
target_link_libraries(Renderer ${GLEW_LIBRARY})