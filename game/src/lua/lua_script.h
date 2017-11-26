#ifndef LUA_SCRIPT_H_
#define LUA_SCRIPT_H_

#include <string>
#include <vector>
#include <functional>
#include "util/common.h"
using std::string;
using std::vector;
using std::function;

extern "C" {

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

}

class LuaScript {
 public:
  typedef function<void()> ForEachEntryFunc;
  LuaScript();
  LuaScript(lua_State*);
  ~LuaScript();
  void LogError(const string&);
  void LogWarning(const string&);
  void LogDebug(const string&);
  void ForEachTableEntry(const string&, ForEachEntryFunc);
  void Run(const string&);
  void Register(const string&, lua_CFunction);

  void PopStack(unsigned n) {
    lua_pop(L, n);
  }

  void CleanStack() {
    int n = lua_gettop(L);
    lua_pop(L, n);
  }

  // Call functions with variadic template
  template<typename R>
  R Call(const string& name) {
    GetToStack(name); // XXX should pop more when name is nested
    return Call<R>(0);
  }

  template<typename R, typename... Args>
  R Call(const string& name, Args... args) {
    GetToStack(name); // XXX should pop more when name is nested
    return Call<R>(0, args...);
  }

  template<typename T>
  vector<T> GetVector(const string& name = "") {
    vector<T> vec;
    ForEachTableEntry(name, [=, &vec] () mutable {
      T val = this->GetTop<T>();
      vec.push_back(val);
    });
    // FIXME pop or not?
    return vec;
  }

  template<typename T>
  T Get(const string& var_expr = "") {
    ASSERT(L != nullptr);

    GetToStack(var_expr);
    T result = GetTop<T>();  
   
    lua_pop(L, 1);
    return result;
  }

  template<typename T>
  T GetOpt(const string& var_expr = "") {
    ASSERT(L != nullptr);
    
    GetToStackOpt(var_expr);
    T result = GetTopOpt<T>();  
   
    lua_pop(L, 1);
    return result;
  }

  template<typename T>
  void SetGlobal(const string& name, T val) {
    lua_pushnumber(L, (double)val);
    lua_setglobal(L, name.c_str());
  }

  void SetGlobal(const string&, const string&);

  void NewGlobalTable(const string&);

  template<typename T>
  void Set(const string& var_expr, T val) {
    string var = "";
    int level = 0;
    bool from_global = (var_expr[0] == '$');
    for (unsigned int i = from_global ? 1 : 0, size = var_expr.size(); i < size; i++) {
      if (var_expr[i] == '.') { // in the middle
        if (level == 0 && from_global) {
          lua_getglobal(L, var.c_str());
        } else {
          lua_getfield(L, -1, var.c_str());
        }
        if (lua_isnil(L, -1)) {
          lua_pop(L, 1);
          lua_newtable(L);
          if (level == 0 && from_global) {
            lua_setglobal(L, var.c_str());
            lua_getglobal(L, var.c_str());
          } else {
            lua_setfield(L, -2, var.c_str());
            lua_getfield(L, -1, var.c_str());
          }
        } else {
          PushToStack(val);
          lua_setfield(L, -2, var.c_str());
        }
        var = "";
        level++;
      } else {
        var += var_expr[i];
      }
    }
    PushToStack(val);
    if (level == 0 && from_global) {
      lua_setglobal(L, var.c_str());
    } else {
      lua_setfield(L, -2, var.c_str());
    }
    lua_pop(L, level);
  }

  template<typename T>
  void PushToStack(T val) {
    lua_pushnumber(L, (double)val);
  }
  void PushToStack(const string&);
  void PushToStack(lua_CFunction);

  int GetToStack(const string& var_expr, bool optional = false) {
    if (var_expr.size() == 0) return 0;

    string var = "";
    int level = 0;
    bool from_global = (var_expr[0] == '$');
    for (unsigned int i = from_global ? 1 : 0, size = var_expr.size(); i < size + 1; i++) {
      if (i == size || var_expr[i] == '.') {
        if (level == 0 && from_global) {
          lua_getglobal(L, var.c_str());
        } else {
          lua_getfield(L, -1, var.c_str());
        }
        
        if (lua_isnil(L, -1)) {
          if (!optional) LogError(var + " is not defined");
          lua_pop(L, level);
          if (!optional) throw "GetToStack Error";
          return 0;
        }
        var = "";
        level++;
      } else {
        var += var_expr[i];
      }
    }

    return level;
  }

  int GetToStackOpt(const string& var_expr) {
    return GetToStack(var_expr, true);
  }

  template<typename T>
  T GetTop() {
    if(!lua_isnumber(L, -1)) {
      LogError("Not a number.");
      throw "Not a number";
    }
    return (T)lua_tonumber(L, -1);
  }

  template<typename T>
  T GetTopOpt() {
    if(!lua_isnumber(L, -1)) {
      LogDebug("Not a number. Returning default value.");
      return GetDefault<T>();
    }
    return (T)lua_tonumber(L, -1);
  }

  template<typename T>
  T GetDefault() { return 0; }

  void DumpStack() {
#ifdef DEBUG
		int i = lua_gettop(L);
		printf("--------------- Stack Dump ----------------\n");
		while (i) {
			int t = lua_type(L, i);
			switch (t) {
				case LUA_TSTRING:
					printf("%d:`%s'", i, lua_tostring(L, i));
					break;
				case LUA_TBOOLEAN:
					printf("%d: %s",i,lua_toboolean(L, i) ? "true" : "false");
					break;
				case LUA_TNUMBER:
					printf("%d: %g",  i, lua_tonumber(L, i));
					break;
				default: printf("%d: %s", i, lua_typename(L, t)); break;
			}
      printf("\n");
			i--;
		}
		printf("--------------- Stack Dump Finished ---------------\n");
#endif // DEBUG
	} 

  void SetRawPointerToGlobal(const string&, void*);

 private:
  template<typename R>
  R Call(unsigned argc) {
    if (lua_pcall(L, argc, 1, 0)) {
      LogError("Error on Call");
    }
    R ret = (R)GetTop<R>();
    lua_pop(L, 1);
    return ret;
  }

  template<typename R, typename... Args>
  R Call(unsigned argc, int arg, Args... args) {
    lua_pushnumber(L, (double)arg);
    return Call<R>(argc + 1, args...);
  }

  template<typename R, typename... Args>
  R Call(unsigned argc, const string& arg, Args... args) {
    lua_pushstring(L, arg.c_str());
    return Call<R>(argc + 1, args...);
  }

 private:
  lua_State* L;
  bool       destroy_;
};

// Template Specializations

template<>
bool LuaScript::GetDefault<bool>();

template<> 
bool LuaScript::GetTop<bool>();

template<> 
bool LuaScript::GetTopOpt<bool>();

template<>
string LuaScript::GetDefault<string>();

template<>
string LuaScript::GetTop<string>();

template<>
string LuaScript::GetTopOpt<string>();

template<>
void LuaScript::Call<void>(unsigned argc);

#endif // LUA_SCRIPT_H_