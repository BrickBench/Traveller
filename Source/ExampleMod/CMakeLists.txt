set(EXAMPLEMOD_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/DllMain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ExampleMod.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp")

set(EXAMPLEMOD_INCLUDE_DIRECTORIES
	"${CMAKE_SOURCE_DIR}/Include/Traveller"
	"${CMAKE_SOURCE_DIR}/Include/ExampleMod")

set(EXAMPLEMOD_LINK_LIBRARIES
	dinput8)

add_library(ExampleMod SHARED ${EXAMPLEMOD_SOURCE_FILES})

target_include_directories(ExampleMod PUBLIC ${EXAMPLEMOD_INCLUDE_DIRECTORIES})

target_link_libraries(ExampleMod PUBLIC ${EXAMPLEMOD_LINK_LIBRARIES})

# TODO: Add way to configure this
install(TARGETS ExampleMod DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}/plugins")