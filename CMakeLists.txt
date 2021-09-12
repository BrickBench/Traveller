cmake_minimum_required(VERSION 3.10)

project(Traveller VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")

set(TRAVELLER_INSTALL_DIRECTORY "C:/Program Files (x86)/Steam/steamapps/common/Lego Star Wars Saga/" CACHE STRING "The directory that CMake will install to.")

# Third Party
add_subdirectory("ThirdParty")

# Internal
add_subdirectory("Source")