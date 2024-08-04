cmake_minimum_required(VERSION 3.5)
project(rivn)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

set(RIVN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
message(STATUS "Output directory: ${RIVN_OUTPUT_DIRECTORY}")

# Thirdparty code
set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/lib")

#
# Lua Library
#
set(LUA_DIR "${THIRDPARTY_DIR}/lua/src")

add_library(lua SHARED
	"${LUA_DIR}/lapi.c"
	"${LUA_DIR}/lauxlib.c"
	"${LUA_DIR}/lbaselib.c"
	"${LUA_DIR}/lcode.c"
	"${LUA_DIR}/lcorolib.c"
	"${LUA_DIR}/lctype.c"
	"${LUA_DIR}/ldblib.c"
	"${LUA_DIR}/ldebug.c"
	"${LUA_DIR}/ldo.c"
	"${LUA_DIR}/ldump.c"
	"${LUA_DIR}/lfunc.c"
	"${LUA_DIR}/lgc.c"
	"${LUA_DIR}/linit.c"
	"${LUA_DIR}/liolib.c"
	"${LUA_DIR}/llex.c"
	"${LUA_DIR}/lmathlib.c"
	"${LUA_DIR}/lmem.c"
	"${LUA_DIR}/loadlib.c"
	"${LUA_DIR}/lobject.c"
	"${LUA_DIR}/lopcodes.c"
	"${LUA_DIR}/loslib.c"
	"${LUA_DIR}/lparser.c"
	"${LUA_DIR}/lstate.c"
	"${LUA_DIR}/lstring.c"
	"${LUA_DIR}/lstrlib.c"
	"${LUA_DIR}/ltable.c"
	"${LUA_DIR}/ltablib.c"
	"${LUA_DIR}/ltm.c"
	"${LUA_DIR}/lundump.c"
	"${LUA_DIR}/lutf8lib.c"
	"${LUA_DIR}/lvm.c"
	"${LUA_DIR}/lzio.c"
)

set_target_properties(lua PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}"
)

target_compile_definitions(lua PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

if(WIN32)
    set_target_properties(lua PROPERTIES PREFIX "")
    set_target_properties(lua PROPERTIES IMPORT_PREFIX "")
endif()

target_compile_definitions(lua PRIVATE LUA_BUILD_AS_DLL)

include_directories(${LUA_DIR})

#
# Rivn
# Executalb
#

set(RIVN_FILES)

include(AddSources)

add_sources(RIVN_FILES "./" c cpp cxx cc "c++" C)  # Source files
add_sources(RIVN_FILES "./" h hpp hxx hh "h++" H inl)  # Header files
add_sources(RIVN_FILES "./" tpp txx tcc)  # Template implementations

include_directories(include/)

add_executable(${PROJECT_NAME}
	${RIVN_FILES} 
)

target_link_directories(${PROJECT_NAME} PRIVATE "${RIVN_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
target_link_libraries(${PROJECT_NAME} lua)

target_compile_definitions(${PROJECT_NAME} PRIVATE
                                $<$<CONFIG:Debug>:_DEBUG>
                            )

# Organise the files in a source group
source_group(TREE "${PROJECT_SOURCE_DIR}/" PREFIX "Files" FILES ${RIVN_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}"
)


