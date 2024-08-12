project(lua-files)

# Find all Lua files in the source directory
set(CT_RIVN_LUA_MODULES)
file(GLOB_RECURSE CT_RIVN_LUA_MODULES RELATIVE "${PROJECT_SOURCE_DIR}" "./modules/*.lua")

set(CT_RIVN_LUA_GENERATORS)
file(GLOB_RECURSE CT_RIVN_LUA_GENERATORS RELATIVE "${PROJECT_SOURCE_DIR}" "./generators/*.lua")


function(copy_directory_rule rule_name source_dir target_dir)
	add_custom_command(
        OUTPUT ${target_dir}
        # Create lua directories
        COMMAND ${CMAKE_COMMAND} -E make_directory "${target_dir}"
        
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        	"${source_dir}"
        	"${target_dir}"
        	
        COMMENT "Copying files to ${target_dir}"
        VERBATIM
    )
    
	# Custom target that depends on the copied files
    add_custom_target(copy_${rule_name} ALL
        DEPENDS ${target_dir}
    )
    
    set_target_properties(copy_${rule_name} PROPERTIES FOLDER "copy_rules")

endfunction()

# Combine all Lua files
set(RIVN_LUA_FILES ${CT_RIVN_LUA_GENERATORS} ${CT_RIVN_LUA_MODULES} ${CT_RIVN_LUA_ROOT})

set(LUA_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")

copy_directory_rule(generators "${PROJECT_SOURCE_DIR}/generators" "${LUA_OUTPUT_DIRECTORY}/generators")
copy_directory_rule(modules "${PROJECT_SOURCE_DIR}/modules" "${LUA_OUTPUT_DIRECTORY}/modules")

# Create custom targets for IDEs
add_custom_target(modules SOURCES ${CT_RIVN_LUA_MODULES})
add_custom_target(generators SOURCES ${CT_RIVN_LUA_GENERATORS})

set_target_properties(modules PROPERTIES FOLDER "lua")
set_target_properties(generators PROPERTIES FOLDER "lua")