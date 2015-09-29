#pragma once

#include <iostream>
#include <stdexcept>
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

void test()
{
    // =================================================================
    // LUA

    lua_State* L = lua_open();
    luaL_openlibs(L);
    luaL_dofile(L, "scripts/init.lua");

    // Retrieve some data
    lua_getglobal(L, "position");
    if (!lua_istable(L, -1))
        throw std::runtime_error("'vertex' is not a valid table");
    double x = getindex(L, "x");
    double y = getindex(L, "y");
    std::cout << x << std::endl;
    std::cout << y << std::endl;
    lua_pop(L, 1);

    // Draw triangle
    struct vertex {
        float x, y, z, r, g, b, u ,v;
    };
    lua_getglobal(L, "vertices");
    if (!lua_istable(L, -1))
        throw std::runtime_error("'vertices' is not a valid table");
    const int size = getindex(L, "size");
    vertex vertices[size];
    for (int i = 0; i < size; i++)
    {
        lua_rawgeti(L, 1, i+1);
        vertices[i].x = getindex(L, "x");
        vertices[i].y = getindex(L, "y");
        vertices[i].r = getindex(L, "r");
        vertices[i].g = getindex(L, "g");
        vertices[i].b = getindex(L, "b");
        std::cout << i << ": { x=" << vertices[i].x << ", y=" << vertices[i].y << " }\n";
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    std::cout << L << std::endl;
    lua_close(L);
}
