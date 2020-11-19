cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Directx11 LANGUAGES CXX)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

#define solution folder structure
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/spdlog)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(SandBox)
add_subdirectory(WhiteBoard)

set_target_properties(
    spdlog
    PROPERTIES 
    FOLDER Vendor
)
