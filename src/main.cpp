#include <iostream>
#include <filesystem>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}


void printUsage();


int main(int argc, char** argv) {
	std::vector<std::string> args(argv, argv + argc);

	if (args.size() < 2) {
		printUsage();
		return 1;
	}

	if (args.size() < 3) {
		std::cout << "\tError: Not enough arguments" << std::endl;
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
		std::cout << "\tConfiguration: \"" << configName << "\" = \"" << projectConfiguration << "\"\n";
		configName.clear();
	}


	
	try
	{
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		
		int result = luaL_dofile(L, luaFileName.c_str());
		
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
	std::cout << "Usage: rivn <lua-file-name> <project-name> [<project-configuration> ...]" << std::endl;
	std::cout << "Example: rivn cmake.lua my_cmake_project" << std::endl;
	std::cout << "rivn will create a new project in the current directory with the name of <project-name>" << std::endl;
	std::cout << "rivn will then call the lua script <lua-file-name> with the project information to configure the project" << std::endl;
}
