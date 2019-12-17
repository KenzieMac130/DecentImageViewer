set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DUNICODE -D_UNICODE)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(RENDERER "OpenGL" CACHE STRING "Rendering back-end")
set(RenderBackends "OpenGL")
set_property(CACHE RENDERER PROPERTY STRINGS ${RenderBackends})

if(RENDERER STREQUAL "OpenGL")
	if(NOT GLEW_INCLUDE_DIR)
	set (GLEW_INCLUDE_DIR "" CACHE PATH "GLEW Include")
	endif()

	if(NOT GLEW_LIBRARY)
	set (GLEW_LIBRARY "" CACHE FILEPATH "GLEW Library")
	endif()
	
	# option(GLEW_STATIC "Use Static GLEW" ON)
	# if(GLEW_STATIC)
		# #todo: static version optional
	# endif()

	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)

	include_directories(${OPENGL_INCLUDE_DIRS})
	include_directories(${GLEW_INCLUDE_DIR})
endif()


#SDL
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#Common utilities
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)
link_libraries(Common)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Renderer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Testing)