# Help me I'am Under The Water
# Copyright 2025 Daynlight
# Licensed under the GNU General, Version 3.0.
# See LICENSE file for details.



cmake_minimum_required(VERSION 3.15)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(PRODUCTION "Enable production build flags" OFF)
# option(SANDBOX_SCRIPTS "Enable script sandboxing build flags" ON)



execute_process(
  COMMAND git submodule update --init --recursive
)



add_subdirectory(vendor/cmrc)
add_subdirectory(vendor/CWindow)

file(GLOB_RECURSE GameDataSet CONFIGURE_DEPENDS "GameData/**")
cmrc_add_resource_library(GameData ${GameDataSet})



add_subdirectory(UnderTheWater)

if(PRODUCTION)
  target_compile_definitions(UnderTheWater PRIVATE PRODUCTION)
endif()

# if(SANDBOX_SCRIPTS)
#   target_compile_definitions(UnderTheWater PRIVATE SANDBOX_SCRIPTS)
# endif()
