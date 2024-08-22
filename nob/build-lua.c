#ifndef BUILD_LUA_H
#define BUILD_LUA_H
#include "../nob.h"

int build_lua(Nob_Cmd *cmd, nob_cl_args* args) 
{
    nob_cmd_append(cmd, "; echo \"Hello\"");

    

    return 0;
}

#endif