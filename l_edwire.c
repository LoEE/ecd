/// ## Necessary declarations
// lua
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <fixint.h>
#include <edwire.h>

static int l_zigzag32enc (lua_State *L)
{
  s32 in = luaL_checkinteger (L, 1);

  u32 out = zigzag32enc (in);

  lua_pushnumber (L, out);

  return 1;
}

static int l_zigzag32dec (lua_State *L)
{
  u32 in = luaL_checkinteger (L, 1);

  s32 out = zigzag32dec (in);

  lua_pushnumber (L, out);

  return 1;
}

static int l_zigzag64enc (lua_State *L)
{
  s64 in = luaL_checkinteger (L, 1);

  u64 out = zigzag64enc (in);

  lua_pushnumber (L, out);

  return 1;
}

static int l_zigzag64dec (lua_State *L)
{
  u64 in = luaL_checkinteger (L, 1);

  s64 out = zigzag64dec (in);

  lua_pushnumber (L, out);

  return 1;
}

#define E(lua_name) { #lua_name, CONCAT(l_, lua_name) }

static const struct luaL_reg funcs[] = {
  E(zigzag32enc),
  E(zigzag32dec),
  E(zigzag64enc),
  E(zigzag64dec),
  {NULL,        NULL       },
};

int luaopen_edwire (lua_State *L)
{
  luaL_register (L, "edwire", funcs);
  return 1;
}
