#include "lua_script.h"

#include <stdio.h>

namespace lua {

LuaScript::LuaScript() : L(nullptr), destroy_(true) {
  L = luaL_newstate();
  if (L != nullptr) luaL_openlibs(L);
}

LuaScript::LuaScript(lua_State* L) : L(L), destroy_(false) {
}

LuaScript::~LuaScript() {
	if (L != nullptr && destroy_) lua_close(L);
}

void LuaScript::Run(const string& filename) {
  if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
    printf("LuaError: Load and run '%s' failed. \n", filename.c_str());
  }
}

void LuaScript::Register(const string& name, lua_CFunction fn) {
  lua_register(L, name.c_str(), fn);
}

void LuaScript::LogError(const string& msg) {
  DumpStack();
  LOGM_ERROR(Lua, "%s", msg.c_str());
}

void LuaScript::LogWarning(const string& msg) {
  LOGM_WARNING(Lua, "%s", msg.c_str());
}

void LuaScript::LogDebug(const string& msg) {
  LOGM_DEBUG(Lua, "%s", msg.c_str());
}

void LuaScript::ForEachTableEntry(const string& name, ForEachEntryFunc cb) {
  int num_stack = GetToStack(name.c_str());
  if (!lua_istable(L, -1)) { // Table not found
    return;
  }
  lua_pushnil(L);
  // TODO handle when table has no entry
  while (lua_next(L, -2)) {
    cb();
    lua_pop(L, 1);
  }
  PopStack(num_stack); // Pop tables pushed by GetToStack
}

void LuaScript::PushToStack(const string& s) {
  lua_pushstring(L, s.c_str());
}

void LuaScript::PushToStack(lua_CFunction fn) {
  lua_pushcfunction(L, fn);
}

void LuaScript::SetGlobal(const string& name, const string& val) {
  lua_pushstring(L, val.c_str());
  lua_setglobal(L, name.c_str());
}

void LuaScript::DumpStack() {
#ifdef DEBUG
  int i = lua_gettop(L);
  printf("--------------- Stack Dump Begin ----------------\n");
  while (i) {
    int t = lua_type(L, i);
    switch (t) {
      case LUA_TSTRING:
        printf("%d:\"%s\"", i, lua_tostring(L, i));
        break;
      case LUA_TBOOLEAN:
        printf("%d: %s", i, lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:
        printf("%d: %g", i, lua_tonumber(L, i));
        break;
      default:
        printf("%d: %s", i, lua_typename(L, t));
        break;
    }
    printf("\n");
    i--;
  }
  printf("--------------- Stack Dump End   ---------------\n");
#endif // DEBUG
}

template<>
void LuaScript::Call<void>(unsigned argc) {
  if (lua_pcall(L, argc, 0, 0)) {
    LogError("Error on Call(return type : void).");
  }
}

void LuaScript::NewGlobalTable(const string& name) {
  lua_newtable(L);
  lua_setglobal(L, name.c_str());
}

} // namespace lua
