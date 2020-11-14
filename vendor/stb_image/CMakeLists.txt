project(stb_image LANGUAGES CXX)

set(project_headers 
    stb_image.h
)

set(project_source 
    stb_image.cpp
)

add_library(
    ${PROJECT_NAME}
    STATIC
    ${project_source}
    ${project_headers}
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
    CXX_STANDARD 17
)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    FOLDER Vendor
)