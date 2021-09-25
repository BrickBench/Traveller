cmake_minimum_required(VERSION 3.10)

project(Traveller VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

set(LUA_SOURCE "lua-5.4.3")
set(TRAVELLER_INSTALL_DIRECTORY "F:/LEGO Files/LEGO Star Wars - Copy/" CACHE STRING "The directory that CMake will install to.")

# Third Party
add_subdirectory("ThirdParty")

# Internal
add_subdirectory("Source")