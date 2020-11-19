project(Engine LANGUAGES CXX)

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=__FILE__+${SOURCE_PATH_SIZE}")

set(project_headers 
    src/Common/Camera.h
    src/Common/Transform.h
	src/Common/Timestep.h
	src/Common/Light.h
	src/Common/Material.h
    src/Core/ModuleCore.h
    src/Core/Base.h
    src/Core/Log.h
    src/File/FbxLoader.h
    src/File/FileCommon.h
    src/File/Serialization.h
    src/File/SerializedData.h
    src/Model/Model.h
    src/Model/3D/Modelbuilder.h
    src/Model/3D/SkeletalAnimation.h
    src/Model/3D/Skeleton.h
    src/Model/2D/Sprite.h
    src/Model/2D/SpriteAnimation.h
    src/Renderer/CBuffer.h
    src/Renderer/Dx11Core.h
    src/Renderer/InputLayout.h
    src/Renderer/ModelBuffer.h
    src/Renderer/PipelineController.h
    src/Renderer/Renderer.h
    src/Renderer/Shader.h
    src/Renderer/Texture.h
    src/Engine.h
    src/pch.h
)

set(project_source 
	src/Core/Log.cpp
    src/Common/Camera.cpp
    src/Common/Transform.cpp
	src/Common/Timestep.cpp
	src/Common/Light.cpp
	src/Common/Material.cpp
    src/Core/ModuleCore.cpp
    src/File/FbxLoader.cpp
    src/File/FileCommon.cpp
    src/Model/Model.cpp
    src/Model/3D/Modelbuilder.cpp
    src/Model/3D/SkeletalAnimation.cpp
    src/Model/3D/Skeleton.cpp
    src/Model/2D/Sprite.cpp
    src/Model/2D/SpriteAnimation.cpp
    src/Renderer/CBuffer.cpp
    src/Renderer/Dx11Core.cpp
    src/Renderer/InputLayout.cpp
    src/Renderer/ModelBuffer.cpp
    src/Renderer/PipelineController.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Shader.cpp
    src/Renderer/Texture.cpp
    src/pch.cpp
)


add_library(
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
    stb_image
    spdlog
)

target_precompile_headers(
	${PROJECT_NAME} 
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h"
)

target_include_directories(
	${PROJECT_NAME} 
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/vendor/Boost"
	"${CMAKE_SOURCE_DIR}/vendor/FbxSDK/include"
	"${CMAKE_SOURCE_DIR}/vendor/stb_image"
	"${CMAKE_SOURCE_DIR}/vendor/spdlog/include"
)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    FOLDER Directx11
)