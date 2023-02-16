set(EXAMPLEMOD_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/DllMain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/ExampleMod.cpp")

add_library(ExampleMod SHARED ${EXAMPLEMOD_SOURCE_FILES})

target_include_directories(ExampleMod PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")

target_link_libraries(ExampleMod PUBLIC DINPUT8)

# TODO: Add way to configure this
install(TARGETS ExampleMod DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}/plugins")
