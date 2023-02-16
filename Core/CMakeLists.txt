
set(TRAVELLER_SOURCE_FILES
 "${CMAKE_CURRENT_SOURCE_DIR}/Source/DllMain.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/Source/LuaMod.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/CoreMod.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/GuiManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/InjectionManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/InputHandler.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/MemWriteUtils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Traveller.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/Source/Configuration.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/Source/LuaRegistry.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/DINPUT8.def"
 )

set(TRAVELLER_INCLUDE_DIRECTORIES
 "${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/ini-parser-cpp/src")

set(TRAVELLER_LINK_LIBRARIES
 imgui
	minhook
	sol2
 lua
)

add_library(DINPUT8 SHARED ${TRAVELLER_SOURCE_FILES})

target_compile_definitions(DINPUT8 PUBLIC WIN32_LEAN_AND_MEAN TTSLLibBuild SOL_USING_CXX_LUA=1 SOL_EXCEPTIONS_SAFE_PROPAGATION)

target_include_directories(DINPUT8 PUBLIC ${TRAVELLER_INCLUDE_DIRECTORIES})

target_link_libraries(DINPUT8 PUBLIC ${TRAVELLER_LINK_LIBRARIES})

# TODO: Add way to configure this
install(TARGETS DINPUT8 DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}")
install(FILES "${CMAKE_SOURCE_DIR}/ThirdParty/fennel.lua" DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}")
