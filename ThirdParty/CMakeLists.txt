add_subdirectory("minhook")
add_subdirectory("sol2")

set(LUA_SOURCE_FILES
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

add_library(lua STATIC ${LUA_SOURCE_FILES})
target_compile_definitions(lua PUBLIC WIN32_LEAN_AND_MEAN)

target_include_directories(lua PUBLIC 
  "${CMAKE_SOURCE_DIR}/ThirdParty/lua-5.4.3/src")

add_library(imgui STATIC  
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_dx9.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_win32.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/misc/cpp/imgui_stdlib.cpp"
  )

target_compile_definitions(imgui PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)

target_include_directories(imgui PUBLIC 
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends"
  "${CMAKE_SOURCE_DIR}/ThirdParty/imgui")
