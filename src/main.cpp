#include <iostream>
#include <filesystem>
#include <unordered_map>


namespace fs = std::filesystem;


#include "Instance.h"

int main(int argc, char** argv) {
	Instance& instance = Instance::getInstance();
	try
	{
		instance.setup(argc, argv);

		std::cout << "\n";


		// 	sol::state lua;
		// 	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::utf8);
		// 	sol::load_result lua_file = lua.load_file(project.luaFilePath.string());
		// 	if (!lua_file.valid()) {
		// 		throw std::runtime_error("Error loading lua file \"" + project.luaFilePath.string() + "\"\n");
		// 	}
		// 	
		// 	sol::table project_table = lua.create_named_table("project");
		// 	project_table["name"] = project.name;
		// 	project_table["path"] = project.projectPath.string();
		// 	project_table["configurations"] = lua.create_table();
		// 	for (const auto& [key, value] : project.configurations) {
		// 		project_table["configurations"][key] = value;
		// 	}
		// 	
		// 	lua_file();
		
		return 0;
	}
	catch (const std::runtime_error& e) {
		std::cout << conmod::make_fg<245, 5, 5>() << "\nRuntime Error: " << CONUP_RESET << e.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cout << conmod::make_fg<245, 5, 5>() << "\nError: " << CONUP_RESET << e.what() << '\n';
	}
	catch (...) {
		std::cout << conmod::make_fg<245, 5, 5>() << "\nUnknown error" << CONUP_RESET << '\n';
	}
	std::cout << '\n';
	Instance::getInstance().printUsage();
	Instance::destroyInstance();
	return 0;
}