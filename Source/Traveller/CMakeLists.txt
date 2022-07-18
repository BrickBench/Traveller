set(LUA_SOURCE_FILES
 "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_win32.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lapi.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lauxlib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lbaselib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lcode.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lcorolib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lctype.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ldblib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ldebug.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ldo.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ldump.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lfunc.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lgc.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/linit.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/liolib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/llex.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lmathlib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lmem.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/loadlib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lobject.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lopcodes.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/loslib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lparser.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lstate.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lstring.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lstrlib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ltable.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ltablib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/ltm.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lua.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/luac.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lundump.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lutf8lib.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lvm.c"
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src/lzio.c")

  SET_SOURCE_FILES_PROPERTIES(${LUA_SOURCE_FILES} PROPERTIES LANGUAGE CXX)

set(TRAVELLER_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/DllMain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/CoreMod.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/GuiManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/InjectionManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/InputHandler.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/MemWriteUtils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ScriptingLibrary.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/Configuration.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/DINPUT8.def"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_dx9.cpp"
	${LUA_SOURCE_FILES}
 )

set(TRAVELLER_INCLUDE_DIRECTORIES
	"${CMAKE_SOURCE_DIR}/Include/Traveller"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends"
	"${CMAKE_SOURCE_DIR}/ThirdParty/sol2/include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/minhook/src"
	"${CMAKE_SOURCE_DIR}/ThirdParty/minhook/include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/ini-parser-cpp/src"
	"${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src")

set(TRAVELLER_LINK_LIBRARIES
	minhook
	sol2)

add_library(DINPUT8 SHARED ${TRAVELLER_SOURCE_FILES})

target_compile_definitions(DINPUT8 PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX TTSLLibBuild SOL_USING_CXX_LUA=1 SOL_EXCEPTIONS_SAFE_PROPAGATION)

target_include_directories(DINPUT8 PUBLIC ${TRAVELLER_INCLUDE_DIRECTORIES})

target_link_libraries(DINPUT8 PUBLIC ${TRAVELLER_LINK_LIBRARIES})

# TODO: Add way to configure this
install(TARGETS DINPUT8 DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}")
install(FILES "${CMAKE_SOURCE_DIR}/ThirdParty/fennel.lua" DESTINATION "${TRAVELLER_INSTALL_DIRECTORY}")
