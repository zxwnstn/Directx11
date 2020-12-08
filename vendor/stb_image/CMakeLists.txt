project(stb_image LANGUAGES CXX)

set(project_headers 
    stb_image.h
	stb_image_resize.h
)

set(project_source 
    stb_image.cpp
    stb_image_resize.cpp
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