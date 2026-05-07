cmake_minimum_required(VERSION 3.15)

project(UnderTheWater LANGUAGES CXX C)



add_executable(UnderTheWater "main.cpp")
target_link_libraries(UnderTheWater CWindow)