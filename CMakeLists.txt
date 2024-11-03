cmake_minimum_required(VERSION 3.5.0)
project(MobitRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_subdirectory(libs/raylib)

# Parser library
file(GLOB PARSER_SOURCES libs/parser/*.cpp)
add_library(parser STATIC ${PARSER_SOURCES})
target_include_directories(parser PUBLIC libs/parser)

# ImGui library
file(GLOB IMGUI_SOURCES libs/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES} libs/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC libs/imgui libs/imgui/backends)

# rlImGui library
file(GLOB RLIMGUI_SOURCES libs/rlimgui/*.cpp)
add_library(rlimgui STATIC ${RLIMGUI_SOURCES})
target_include_directories(rlimgui PUBLIC libs/rlimgui)
target_link_libraries(rlimgui PRIVATE imgui)

# Main files
file(GLOB MAIN_SOURCES src/*.cpp)               # sources
include_directories(include)                    # headers
add_executable(MobitRenderer ${MAIN_SOURCES})

target_link_libraries(MobitRenderer PRIVATE raylib parser imgui rlimgui)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
