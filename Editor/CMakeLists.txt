cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(Editor LANGUAGES CXX)

#Setup Qt5 for msvc2017 x64
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/Qt5_MSVC")
set(QTDIR "${CMAKE_SOURCE_DIR}/vendor/Qt5_MSVC/")

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

#ui
set(project_ui
	src/MainWindow/MainWindow.ui
)

#header
set(project_headers
    src/pch.h
	src/Viewport/Viewport.h
    src/MainWindow/MainWindow.h
	src/Viewport/RenderingThread.h
)

set(moc_headers
	src/Viewport/Viewport.h
    src/MainWindow/MainWindow.h
)

#cpp
set(project_sources 
    src/pch.cpp
    src/main.cpp
	src/Viewport/Viewport.cpp
    src/MainWindow/MainWindow.cpp
	src/Viewport/RenderingThread.cpp
)

#qrc
set(project_qrc    
)

#QtAuto Generate - moc, ui
qt5_wrap_ui(project_headers_wrapped ${project_ui})
qt5_wrap_cpp(project_sources_moc ${moc_headers})

add_executable(
	${PROJECT_NAME} 
	${project_headers} 
	${project_sources}
	${project_qrc} 
	${project_sources_moc} 
	${project_headers_wrapped} 
)

add_custom_command( 
	TARGET ${PROJECT_NAME} 
	COMMAND echo "Create Qt5 dependencies.."
	COMMAND call 
	vendor\\Qt5_MSVC\\bin\\windeployqt.exe
	$(OutputPath)$(TargetName)$(TargetExt)
	${WindpOption}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
    CXX_STANDARD 17
)

#precompile header
target_precompile_headers(
	${PROJECT_NAME} 
	PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h"
)

#include dir
target_include_directories(
	${PROJECT_NAME} 
	PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}"
)

#library
target_link_libraries(
	${PROJECT_NAME}
    PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
	Engine
)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    FOLDER Directx11
)