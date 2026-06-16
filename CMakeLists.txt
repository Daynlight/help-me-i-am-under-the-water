cmake_minimum_required(VERSION 3.15)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

execute_process(
  COMMAND git submodule update --init --recursive
)

add_subdirectory(vendor/cmrc)
add_subdirectory(vendor/CWindow)

file(GLOB_RECURSE GameDataSet CONFIGURE_DEPENDS "GameData/**")
cmrc_add_resource_library(GameData ${GameDataSet})


add_subdirectory(UnderTheWater)

option(PRODUCTION "Enable production build flags" OFF)
if(PRODUCTION)
  add_compile_definitions(PRODUCTION)
endif()