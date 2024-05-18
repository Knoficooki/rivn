project(setup_dll)

set(SETUP_DLL_NAME "setup" CACHE STRING "The name of the executable.")

set(SETUP_SOURCE_FOLDER "src" CACHE STRING "Path to source.")

file(GLOB_RECURSE SETUP_C_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "${SETUP_SOURCE_FOLDER}/*.c")
file(GLOB_RECURSE SETUP_C_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "${SETUP_SOURCE_FOLDER}/*.h")

file(GLOB_RECURSE SETUP_CPP_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "${SETUP_SOURCE_FOLDER}/*.cpp")
file(GLOB_RECURSE SETUP_CPP_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "${SETUP_SOURCE_FOLDER}/*.hpp")

file(GLOB_RECURSE SETUP_CXX_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "${SETUP_SOURCE_FOLDER}/*.cxx")
file(GLOB_RECURSE SETUP_CXX_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "${SETUP_SOURCE_FOLDER}/*.hxx")

include_directories(include/)

set(SETUP_CHEADERS ${SETUP_C_HEADERS} ${SETUP_CPP_HEADERS} ${SETUP_CXX_HEADERS})
set(SETUP_CSOURCES ${SETUP_C_SOURCES} ${SETUP_CPP_SOURCES} ${SETUP_CXX_SOURCES})


# Create the shared library
add_library(setup_dll SHARED ${SOURCES})

# Set the output directory for the shared library
set_target_properties(setup_dll PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
)

# Include directories (if any)
target_include_directories(setup_dll PRIVATE "${CMAKE_SOURCE_DIR}/src/include/")

# Optionally add compile options (e.g., optimization flags
target_compile_options(setup_dll PRIVATE
	$<$<CONFIG:Debug>:-g>
	$<$<CONFIG:Release>:-O3>
)


target_compile_definitions(setup_dll PRIVATE
                                $<$<CONFIG:Debug>:_DEBUG>
                            )

# Organisieren Sie die Dateien in einer Quellgruppe
source_group(TREE "${PROJECT_SOURCE_DIR}/${SETUP_SOURCE_FOLDER}/" PREFIX "Source Files" FILES ${SETUP_CSOURCES})
source_group(TREE "${PROJECT_SOURCE_DIR}/${SETUP_SOURCE_FOLDER}/" PREFIX "Header Files" FILES ${SETUP_CHEADERS})

set_target_properties(setup_dll PROPERTIES OUTPUT_NAME "${SETUP_DLL_NAME}")
set_target_properties(setup_dll PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

