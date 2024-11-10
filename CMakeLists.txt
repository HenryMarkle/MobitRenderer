cmake_minimum_required(VERSION 3.5.0)
project(MobitRenderer VERSION 0.1.0 LANGUAGES C CXX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_subdirectory(libs/raylib)
add_subdirectory(libs/spdlog)
add_subdirectory(libs/MobitParser)

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

target_compile_definitions(MobitRenderer PRIVATE
    APP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    APP_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    APP_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

target_link_libraries(MobitRenderer PRIVATE raylib imgui rlimgui spdlog MobitParser)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
