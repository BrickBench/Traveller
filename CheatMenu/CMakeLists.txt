set(CHEATMENU_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/DllMain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/CheatMenu.cpp")

add_library(CheatMenu SHARED ${CHEATMENU_SOURCE_FILES})

target_include_directories(CheatMenu PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")

target_link_libraries(CheatMenu PUBLIC DINPUT8)

install(TARGETS CheatMenu DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}/plugins")
