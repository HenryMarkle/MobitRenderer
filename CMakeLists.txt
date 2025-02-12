cmake_minimum_required(VERSION 3.15.0)
project(MobitRenderer VERSION 0.1.2 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(NOT(WIN32))
  add_subdirectory(libs/raylib)
endif()
add_subdirectory(libs/spdlog)
add_subdirectory(libs/MobitParser)

# ImGui library
file(GLOB IMGUI_SOURCES libs/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC libs/imgui libs/imgui/backends)

# rlImGui library
file(GLOB RLIMGUI_SOURCES libs/rlimgui/*.cpp)
add_library(rlimgui STATIC ${RLIMGUI_SOURCES})
target_include_directories(rlimgui PUBLIC libs/rlimgui)
target_link_libraries(rlimgui PRIVATE imgui)

# raylib
if (WIN32)
  include_directories(libs/raylib_mingw/include)
  link_directories(libs/raylib_mingw/lib)
endif()

# Main files
include_directories(include)                              # headers

file(GLOB MAIN_SOURCES src/*.cpp)                         # sources
file(GLOB MAIN_DRAW_SOURCES src/draw/*.cpp)               # sources
file(GLOB MAIN_SDRAW_SOURCES src/sdraw/*.cpp)             # sources
file(GLOB MAIN_PAGE_SOURCES src/pages/*.cpp)              # sources
file(GLOB MAIN_SERDE_SOURCES src/serialization/*.cpp)     # sources
file(GLOB MAIN_RENDERER_SOURCES src/renderer/*.cpp)       # sources

if(WIN32)
file(GLOB PLATFORM_IMPL_SOURCES src/platform/windows/*.cpp)
elseif(LINUX)
file(GLOB PLATFORM_IMPL_SOURCES src/platform/linux/*.cpp)
else(APPLE)
endif()

add_executable(
  MobitRenderer 
  ${PLATFORM_IMPL_SOURCES}
  ${MAIN_SOURCES} 
  ${MAIN_DRAW_SOURCES}
  ${MAIN_SDRAW_SOURCES}
  ${MAIN_PAGE_SOURCES}
  ${MAIN_SERDE_SOURCES}
  ${MAIN_RENDERER_SOURCES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Copy assets
  set(PROJECT_SOURCE_ASSETS "${CMAKE_SOURCE_DIR}/Assets")
  set(PROJECT_BINARY_ASSETS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets")

  add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_ASSETS}
        ${PROJECT_BINARY_ASSETS}
    COMMENT "Copying assets folder to binary directory as 'Assets'"
  )
  #

  # Copy data
  set(PROJECT_SOURCE_Data "${CMAKE_SOURCE_DIR}/Data")
  set(PROJECT_BINARY_Data "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Data")

  add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_Data}
        ${PROJECT_BINARY_Data}
    COMMENT "Copying Data folder to binary directory"
  )
  #

  add_dependencies(MobitRenderer copy_assets copy_data)
endif()

if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(MobitRenderer PROPERTIES WIN32_EXECUTABLE YES)
  endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(MobitRenderer PRIVATE IS_DEBUG_BUILD=1)
else()
  target_compile_definitions(MobitRenderer PRIVATE IS_DEBUG_BUILD=0)
endif()

if (FEATURE_DATAPACKS)
  target_compile_definitions(MobitRenderer PRIVATE FEATURE_DATAPACKS=1)
else()
  target_compile_definitions(MobitRenderer PRIVATE FEATURE_DATAPACKS=0)
endif()

if(FEATURE_PALETTES)
  target_compile_definitions(MobitRenderer PRIVATE FEATURE_PALETTES=1)
else()
  target_compile_definitions(MobitRenderer PRIVATE FEATURE_PALETTES=0)
endif()

target_compile_definitions(MobitRenderer PRIVATE BUILD_TYPE="${CMAKE_BUILD_TYPE}")

# Define version
file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_CONTENT)
string(STRIP "${VERSION_CONTENT}" VERSION_CONTENT)
target_compile_definitions(MobitRenderer PRIVATE
  APP_VERSION="${VERSION_CONTENT}")

target_compile_definitions(MobitRenderer PRIVATE APP_NAME="Mobit Renderer")

# target_compile_definitions(MobitRenderer PRIVATE
#   APP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
#   APP_VERSION_MINOR=${PROJECT_VERSION_MINOR}
#   APP_VERSION_PATCH=${PROJECT_VERSION_PATCH}
# )

# Copy imgui.ini
file(COPY ${CMAKE_SOURCE_DIR}/imgui.ini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
# Copy config.toml
file(COPY ${CMAKE_SOURCE_DIR}/config.toml DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

# Embed changelog
set(CHLG_INPUT_FILE "${CMAKE_SOURCE_DIR}/changelog.txt")
set(CHLG_OUTPUT_FILE "${CMAKE_BINARY_DIR}/embedded_changelog.h")

file(READ "${CHLG_INPUT_FILE}" CHANGELOG_CONTENT)
string(REPLACE "\"" "\\\"" CHANGELOG_CONTENT
  "${CHANGELOG_CONTENT}") # Escape double quotes

string(REPLACE "\n" "\\n\"\n\""
  CHANGELOG_CONTENT "${CHANGELOG_CONTENT}") # Handle newlines

file(WRITE "${CHLG_OUTPUT_FILE}" "#pragma once\n")
file(APPEND "${CHLG_OUTPUT_FILE}"
  "constexpr const char* EMBEDDED_CHANGELOG = R\"(\n")
file(APPEND "${CHLG_OUTPUT_FILE}" "${CHANGELOG_CONTENT}")
file(APPEND "${CHLG_OUTPUT_FILE}" "\n)\";\n")

target_include_directories(MobitRenderer PRIVATE "${CMAKE_BINARY_DIR}")
#

# TOML
target_include_directories(MobitRenderer PRIVATE libs/tomlplusplus/include)

if(WIN32)
  target_link_libraries(MobitRenderer PRIVATE ${CMAKE_SOURCE_DIR}/libs/raylib_mingw/lib/libraylib.a)
else()
  target_link_libraries(MobitRenderer PRIVATE raylib)
endif()

target_link_libraries(MobitRenderer PRIVATE MobitParser imgui rlimgui spdlog)

if(WIN32)
  target_link_libraries(MobitRenderer PRIVATE gdi32 winmm)
endif()

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
