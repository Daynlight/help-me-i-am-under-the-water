cmake_minimum_required(VERSION 3.15)

execute_process(
  COMMAND git submodule update --init --recursive
)

add_subdirectory(vendor/cmrc)
add_subdirectory(vendor/CWindow)

add_subdirectory(UnderTheWater)