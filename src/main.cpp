#include <iostream>
#include <filesystem>
#include <vector>
#include <unordered_map>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace fs = std::filesystem;

#ifdef _DEBUG
#define debugPrint(x) std::cout << x << '\n'
#else
#define debugPrint(x)
#endif

void printUsage();

struct Project {
	std::string name;
	std::string luaFileName;
	fs::path luaFilePath;
	fs::path projectPath;
	std::unordered_map<std::string, std::string> configurations;
};

std::ostream& operator<<(std::ostream& os, const Project& config)
{
	os << "Project configuration:\n";
	os << "\tName: " << config.name << '\n';
	os << "\tPath: " << config.projectPath.string() << '\n';
	os << "\tLua file: " << config.luaFileName << '\n';
	os << "\tLua path: " << config.luaFilePath.string() << '\n';
	os << "\tConfigurations:\n";
	for (const auto& [key, value] : config.configurations) {
		os << "\t\t" << key << " = " << value << '\n';
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const sol::table& table) {
	static size_t indent = 0;
	indent++;
	size_t i = 0;
	for (i = 0; i < indent-1; i++)
	{
		os << "\t";
	}
	os << "{\n";
	for (const auto& [key, value] : table) {
		for (i = 0; i < indent; i++) {
			os << "\t";
		}
		os << key << " = " << value << '\n';
	}
	for (i = 0; i < indent-1; i++)
	{
		os << "\t";
	}
	os << "}\n";
	indent--;
	return os;
}

 
Project configureProject(const fs::path& rivnPath, const fs::path& luaModules, int argc, char** argv);

int main(int argc, char** argv) {
	fs::path rivnPath = (fs::current_path() / argv[0]).parent_path().lexically_normal();
    fs::path luaModules = rivnPath / "lua";
	
	debugPrint("\tRivn directory: " << rivnPath.string());
    debugPrint("\tLua modules directory: " << luaModules.string());

	try
	{
		Project project = configureProject(rivnPath, luaModules, argc, argv);

		debugPrint(project);

		std::cout << "\n";


		sol::state lua;
		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::utf8);
		sol::load_result lua_file = lua.load_file(project.luaFilePath.string());
		if (!lua_file.valid()) {
			throw std::runtime_error("Error loading lua file \"" + project.luaFilePath.string() + "\"\n");
		}

		sol::table project_table = lua.create_named_table("project");
		project_table["name"] = project.name;
		project_table["path"] = project.projectPath.string();
		project_table["configurations"] = lua.create_table();
		for (const auto& [key, value] : project.configurations) {
			project_table["configurations"][key] = value;
		}
		
		lua_file();

		int c = getc(stdin);
		
		return 0;
	}
	catch (const std::runtime_error& e) {
		std::cout << "\nRuntime Error: " << e.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cout << "\nError: " << e.what() << '\n';
	}
	catch (...) {
		std::cout << "\nUnknown error" << '\n';
	}
	std::cout << '\n';
	printUsage();
	return 0;
}

void printUsage()
{
	std::cout << "Usage: rivn <lua-file-name> <project-name> [<project-configuration> ...]\n";
	std::cout << "\tExample: rivn c new_c_project --cmake true --build-script false --lua true --compiler-flags \"-O3 -Wall -Wextra\"\n";
	std::cout << "rivn will create a new project in the current directory with the name of <project-name>\n";
	std::cout << "rivn will then call the lua script <lua-file-name> with the project information to configure the project\n";
}

Project configureProject(const fs::path& rivnPath, const fs::path& luaModules, int argc, char** argv)
{
	Project project;
	std::vector<std::string> args(argv, argv + argc);

	if (args.size() < 3) {
		throw std::runtime_error("Not enough arguments");
	}
	
	const std::string luaFileName = args[1];
	const std::string projectName = args[2];
	std::vector<std::string> projectConfigurations;
	if (args.size() > 3)
	{
		projectConfigurations = std::vector(args.begin() + 3, args.end());
	}

	std::string configName;
	for (const auto& projectConf : projectConfigurations) {
		if (projectConf.starts_with("--") && projectConf.size() > 2) {
			if (!configName.empty()) {
				project.configurations[configName] = "ON";
				configName = projectConf.substr(2);
			}
			configName = projectConf.substr(2);
			continue;
		}

		if (configName.empty()) {
			throw std::runtime_error("Unexpected argument \"" + projectConf + "\"\n");
		}
		debugPrint("\tConfiguration: \"" << configName << "\" = \"" << projectConf << "\"");
		project.configurations[configName] = projectConf;
		configName.clear();
	}

	fs::path luaFilePath = luaModules / (luaFileName + ".lua");

	if (!exists(luaFilePath)) {
		throw std::runtime_error("Lua file \"" + luaFilePath.string() + "\" does not exist");
	}

	project.luaFilePath = luaFilePath;
	project.luaFileName = luaFileName;
	project.projectPath = (fs::current_path() / projectName).lexically_normal();
	project.name = projectName;
	return project;
}
