cmake_minimum_required(VERSION 3.15)

execute_process(
  COMMAND git submodule update --init --recursive
)

add_subdirectory(vendor/cmrc)
add_subdirectory(vendor/CWindow)

file(GLOB_RECURSE GameDataSet CONFIGURE_DEPENDS "GameData/**")
cmrc_add_resource_library(GameData ${GameDataSet})


add_subdirectory(UnderTheWater)