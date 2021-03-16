project(SandBox LANGUAGES CXX)

set(project_headers 
    src/SandBox.h
	src/pceh.h
	src/Scene.h
)

set(project_source 
    src/SandBox.cpp
    src/main.cpp
	src/pceh.cpp
	src/Scene.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${project_source}
    ${project_headers}
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
    CXX_STANDARD 17
)

target_link_libraries(
	${PROJECT_NAME}
    PRIVATE
    Engine
)

target_precompile_headers(
	${PROJECT_NAME} 
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/pceh.h"
)

target_include_directories(
	${PROJECT_NAME} 
	PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/Engine/src
	 ${CMAKE_SOURCE_DIR}/vendor/imgui
)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    FOLDER Directx11
)