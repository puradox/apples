#pragma once

#include <iostream>
#include "lua.hpp"

// View the stack of a lua_State
std::ostream& operator<<(std::ostream& stream, lua_State* L)
{
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++)
    {
        int type = lua_type(L, i);
        switch (type)
        {
            case LUA_TSTRING:
                stream << "'" << lua_tostring(L, i) << "'"; break;
            case LUA_TBOOLEAN:
                stream << (lua_toboolean(L, i) ? "true" : "false"); break;
            case LUA_TNUMBER:
                stream << lua_tonumber(L, i); break;
            default:
                stream << lua_typename(L, type); break;
        }
        stream << "   ";
    }
    return stream;
}

static bool check_error(lua_State* L, int status)
{
    if (status != 0)
    {
        std::cerr << "ERROR: " << lua_tostring(L, -1);
        lua_pop(L, 1); // remove error message
        std::cerr << "       STACK: " << L << std::endl;
        return true;
    }
    return false;
}

inline double getindex(lua_State* L, const char* key)
{
    double result = 0;
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (lua_isnil(L, -1))
        std::cerr << "Cannot find key '" << key << "'\n";
    else
        result = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return result;
}

