cmake_minimum_required(VERSION 3.20)
project(rivn)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# Exclude CMake files from appearing in IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# If you're using Visual Studio, you can also add:
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

set(RIVN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIG>")
message(STATUS "Output directory: ${RIVN_OUTPUT_DIRECTORY}")

# Thirdparty code
set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/lib")
include_directories(include/)

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
    ARCHIVE_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/"
    LIBRARY_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/"
    RUNTIME_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/"
)

target_compile_definitions(lua PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

if(WIN32)
    set_target_properties(lua PROPERTIES IMPORT_PREFIX "")
    target_compile_definitions(lua PRIVATE LUA_BUILD_AS_DLL)
endif()

set_target_properties(lua PROPERTIES PREFIX "")


include_directories(${LUA_DIR})

# Optional: Set properties for the target
set_target_properties(lua PROPERTIES FOLDER "libs")

#
# Rivn
# Executable
#


include(AddSources)

set(RIVN_FILES)
add_sources(RIVN_FILES "src/" c cpp cxx cc "c++" C)  # Source files
add_sources(RIVN_FILES "src/" h hpp hxx hh "h++" H inl)  # Header files
add_sources(RIVN_FILES "src/" tpp txx tcc)  # Template implementations

include_directories(include/)

add_executable(${PROJECT_NAME}
    ${RIVN_FILES}
)

target_link_directories(${PROJECT_NAME} PRIVATE "${RIVN_OUTPUT_DIRECTORY}/")
target_link_libraries(${PROJECT_NAME} lua)

target_compile_definitions(${PROJECT_NAME} PRIVATE
                                $<$<CONFIG:Debug>:_DEBUG>
                            )

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/"
)


#
# Copy Lua files
#

# Custom target to copy Lua files
set(LUA_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/lua")
set(LUA_OUTPUT_DIRECTORY "${RIVN_OUTPUT_DIRECTORY}/lua")

message(STATUS "Lua source directory: ${LUA_SOURCE_DIRECTORY}")
message(STATUS "Lua output directory: ${LUA_OUTPUT_DIRECTORY}")

file(GLOB_RECURSE RIVN_LUA_FILES RELATIVE "${LUA_SOURCE_DIRECTORY}" "${LUA_SOURCE_DIRECTORY}/*.lua")
message(STATUS "Lua files: ${RIVN_LUA_FILES}")

# Create a list of full paths for source and destination Lua files
set(RIVN_LUA_SOURCE_FILES)
set(RIVN_LUA_OUTPUT_FILES)
foreach(lua_file ${RIVN_LUA_FILES})
    list(APPEND RIVN_LUA_SOURCE_FILES "${LUA_SOURCE_DIRECTORY}/${lua_file}")
    list(APPEND RIVN_LUA_OUTPUT_FILES "${LUA_OUTPUT_DIRECTORY}/${lua_file}")
endforeach()

# Custom command to copy Lua files
add_custom_command(
    OUTPUT ${RIVN_LUA_OUTPUT_FILES}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUA_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RIVN_LUA_SOURCE_FILES}
        "${LUA_OUTPUT_DIRECTORY}"
    DEPENDS ${RIVN_LUA_SOURCE_FILES}
    COMMENT "Copying Lua files to ${LUA_OUTPUT_DIRECTORY}"
    VERBATIM
)

# Custom target that depends on the copied files
add_custom_target(copy_lua_files ALL
    DEPENDS ${RIVN_LUA_OUTPUT_FILES}
)


##
## RIVN-LUA
##
# Define the source directory for Lua files
set(CT_LUA_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/lua")

# Find all Lua files in the source directory
file(GLOB_RECURSE CT_RIVN_LUA_FILES "${CT_LUA_SOURCE_DIRECTORY}/*.lua")

# Create a custom target for Lua files
add_custom_target(rivn-modules
    SOURCES ${CT_RIVN_LUA_FILES}
)

# Organize Lua files in the IDE
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/lua" PREFIX "Lua Files" FILES ${CT_RIVN_LUA_FILES})

#
# Organizing files in source groups
#

# For your main project files, be explicit about which files to include
source_group(TREE "${PROJECT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${RIVN_FILES})