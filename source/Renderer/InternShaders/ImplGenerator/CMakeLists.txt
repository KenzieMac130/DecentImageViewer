add_executable(ShaderImplCodeGenerator 
${CMAKE_CURRENT_SOURCE_DIR}/cute_files.h
${CMAKE_CURRENT_SOURCE_DIR}/ShaderImplGen.c)
set_property(TARGET ShaderImplCodeGenerator PROPERTY C_STANDARD 99)
set_property(TARGET ShaderImplCodeGenerator PROPERTY FOLDER "DecentImageViewer/CodeGenerators")