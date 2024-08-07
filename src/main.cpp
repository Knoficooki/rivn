#include <iostream>
#include <filesystem>
#include <vector>

#include "lua_helper.hpp"


#ifdef _DEBUG
#define debugPrint(x) std::cout << x << '\n'
#else
#define debugPrint(x)
#endif

void printUsage();

int main(int argc, char** argv) {
	namespace fs = std::filesystem;
	// Test Stuff
	
	fs::path rivnPath = (fs::current_path() / argv[0]).parent_path();
	debugPrint("\tRivn directory: " << rivnPath.string());

	// End Test Stuff

	// Rivn Project
	
	std::vector<std::string> args(argv, argv + argc);

	if (args.size() < 2) {
		printUsage();
		return 1;
	}

	if (args.size() < 3) {
		std::cout << "\tError: Not enough arguments\n";
		printUsage();
		return 1;
	}
	
	const std::string luaFileName = args[1];
	const std::string projectName = args[2];
	std::vector<std::string> projectConfigurations;
	if (args.size() > 3)
	{
		projectConfigurations = std::vector(args.begin() + 3, args.end());
	}

	std::string configName;
	for (const auto& projectConfiguration : projectConfigurations) {
		if (projectConfiguration.starts_with("--") && projectConfiguration.size() > 2) {
			if (!configName.empty()) {
				std::cout << "\tError: Unexpected argument \"" << projectConfiguration << "\" after \"" << configName << "\"\n";
				return 1;
			}
			configName = projectConfiguration.substr(2);
			continue;
		}

		if (configName.empty()) {
			std::cout << "\tError: Unexpected argument \"" << projectConfiguration << "\"\n";
			return 1;
		}
		debugPrint("\tConfiguration: \"" << configName << "\" = \"" << projectConfiguration << "\"");
		configName.clear();
	}

	fs::path luaFilePath = rivnPath / "lua" / (luaFileName + ".lua");

	if (!exists(luaFilePath)) {
		std::cout << "\tError: Lua file \"" << luaFilePath << "\" does not exist\n";
		return 1;
	}
	
	try
	{
		lua_State *L = luaL_newstate();
		luaL_openlibs(L); // Load the standard libraries

		lua::Lua lua;
		int result = lua.doFile(luaFilePath.string());
		lua::LuaVar luaVar = lua.getGlobal("greeting");

		luaVar = lua.getGlobal("player");
		debugPrint("\tLua variable \"player\" = \"" << luaVar.as<lua::LuaType::Table>()["name"].as<lua::LuaType::String>());
		
		if (result != 0) {
			std::cout << "Error: " << lua_tostring(L, -1) << '\n';
			lua_close(L);
			return 1;
		}
		lua_close(L);
	}
	catch (const std::runtime_error& e) {
		std::cout << e.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
	catch (...) {
		std::cout << "Unknown error" << '\n';
	}
	return 0;
}

void printUsage()
{
	std::cout << "Usage: rivn <lua-file-name> <project-name> [<project-configuration> ...]\n";
	std::cout << "Example: rivn cmake.lua my_cmake_project\n";
	std::cout << "rivn will create a new project in the current directory with the name of <project-name>\n";
	std::cout << "rivn will then call the lua script <lua-file-name> with the project information to configure the project\n";
}
