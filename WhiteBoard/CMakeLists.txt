project(WhiteBoard LANGUAGES CXX)

set(project_headers 
    src/Main.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${project_headers}
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(
	${PROJECT_NAME} 
	PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/Boost
)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    FOLDER Directx11
)