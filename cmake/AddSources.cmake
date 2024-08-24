# cmake/AddSources.cmake

function(add_sources var_name path)
    set(sources)
    foreach(ext ${ARGN})
        file(GLOB_RECURSE files RELATIVE ${PROJECT_SOURCE_DIR} "${path}/*.${ext}")
        list(APPEND sources ${files})
    endforeach()
    set(${var_name} ${${var_name}} ${sources} PARENT_SCOPE)
endfunction()