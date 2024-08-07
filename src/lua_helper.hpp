#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <unordered_map>
#include <string>

namespace lua
{
	enum class LuaType {
		Nil,
		Number,
		String,
		Table,
		Function,
		Boolean
	};
	

	class LuaVar
	{
		LuaType m_type;
		union {
			bool m_bool;
			int m_int;
			double m_double;
			std::string m_string;
			std::string m_functionName;
			std::unordered_map<std::string, LuaVar> m_table;
		};
	public:
		friend class Lua;
		LuaVar(LuaType type) : m_type(type) {}
		LuaVar(int value) : m_type(LuaType::Number), m_int(value) {}
		LuaVar(double value) : m_type(LuaType::Number), m_double(value) {}
		LuaVar(const std::string& value) : m_type(LuaType::String), m_string(value) {}
		LuaVar(lua_State* L) {
			if (lua_isnil(L, -1)) {
				lua_pop(L, 1);
				m_type = LuaType::Nil;
				return;
			}
			if (lua_isboolean(L, -1)) {
				lua_pop(L, 1);
				m_type = LuaType::Boolean;
				m_bool = lua_toboolean(L, -1);
				return;
			}
			if (lua_isnumber(L, -1)) {
				lua_pop(L, 1);
				m_type = LuaType::Number;
				m_double = lua_tonumber(L, -1);
				return;
			}
			if (lua_isstring(L, -1)) {
				lua_pop(L, 1);
				m_type = LuaType::String;
				m_string = lua_tostring(L, -1);
				return;
			}
			if (lua_istable(L, -1)) {
				lua_pop(L, 1);
				m_type = LuaType::Table;
				m_table.clear();
				lua_pushnil(L);
				while (lua_next(L, -2) != 0) {
					m_table[lua_tostring(L, -2)] = LuaVar(L); // error: No assignment operator
					lua_pop(L, 1);								// TODO: add assignment operator
				}
				return;
			}
			// TODO: Functions
			throw std::runtime_error("Unknown Lua type");
		}
		~LuaVar() {}

		LuaType getType() const { return m_type; }

		template<LuaType T>
		auto as() const -> std::enable_if_t<T == LuaType::Number, int>
		{
			if (m_type != T)
				throw std::runtime_error("LuaVar is not a number");
			return m_int;
		}

		template<LuaType T>
		auto as() const -> std::enable_if_t<T == LuaType::String, std::string>
		{
			if (m_type != T)
				throw std::runtime_error("LuaVar is not a string");
			return m_string;
		}

		template<LuaType T>
		auto as() const -> std::enable_if_t<T == LuaType::Table, std::unordered_map<std::string, LuaVar>>
		{
			if (m_type != T)
				throw std::runtime_error("LuaVar is not a table");
			return m_table;
		}

		template<LuaType T>
		auto as() const -> std::enable_if_t<T == LuaType::Function, std::string>
		{
			if (m_type != T)
				throw std::runtime_error("LuaVar is not a function");
			return m_functionName;
		}

		template<LuaType T>
		auto as() const -> std::enable_if_t<T == LuaType::Boolean, bool>
		{
			if (m_type != T)
				throw std::runtime_error("LuaVar is not a boolean");
			return m_bool;
		}

		template<LuaType T>
		auto as(std::string key) const -> std::enable_if_t<T == LuaType::Table, LuaVar>
		{
			if (m_type != T)
				throw std::runtime_error("LuaVar is not nil");
			return m_table.at(key);
		}
		
	};

	class Lua
	{
		lua_State *L;
	public:
		Lua() {
			L = luaL_newstate();
			luaL_openlibs(L); // Load the standard libraries
		}

		~Lua() {
			lua_close(L);
		}

		int doFile(const std::string& fileName) const {
			return luaL_dofile(L, fileName.c_str());
		}

		int doString(const std::string& str) const {
			return luaL_dostring(L, str.c_str());
		}
	
		void setGlobal(const std::string& name, const std::string& value) const
		{
			doString(name + " = " + value);
		}


		LuaVar getGlobal(const std::string& name) const
		{
			lua_getglobal(L, name.c_str());

			LuaVar result(L);
			lua_pop(L, 1);
			return result;
		}
	};
}