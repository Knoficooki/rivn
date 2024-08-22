#ifndef BUILD_LUA_FILES_COPY_H
#define BUILD_LUA_FILES_COPY_H
#include "../nob.h"

int build_lua_files_copy(Nob_Cmd *cmd, nob_cl_args* args) 
{
    nob_cmd_append(cmd, "; echo \"Hello\"");

    

    return 0;
}
#endif