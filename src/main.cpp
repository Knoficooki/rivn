#include <iostream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <exception>
#include <stdexcept>

namespace fs = std::filesystem;

#ifdef _DEBUG
#define debugPrint(x) std::cout << x << '\n'
#else
#define debugPrint(x)
#endif

namespace fs = std::filesystem;

struct Project {
    std::string name;
    std::string configName;
    fs::path path;
    fs::path configurePath;
    std::unordered_map<std::string, std::string> configs;
};

Project configureProject(int argc, char** argv);

void printUsage();


std::ostream& operator<<(std::ostream& os, const ProjetConfiguration& config)
{
	os << "Project configuration:\n";
	os << "\tName: " << config.projectName << '\n';
	os << "\tPath: " << config.projectPath.string() << '\n';
	os << "\tLua file: " << config.luaFileName << '\n';
	os << "\tLua path: " << config.luaFilePath.string() << '\n';
	os << "\tConfigurations:\n";
	for (const auto& [key, value] : config.configurations) {
		os << "\t\t" << key << " = " << value << '\n';
	}
	return os;
}


int main(int argc, char** argv) {
	fs::path rivnPath = (fs::current_path() / argv[0]).parent_path().lexically_normal();
	debugPrint("\tRivn directory: " << rivnPath.generic_string());

    Project project = configureProject(argc, argv);

	try
	{
        std::cout << "Project Name: " << project.name << "\n";
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
    std::cout << "Press enter to continue... ";
    std::flush(std::cout);
    int c = getc(stdin);
	return 0;
}

void printUsage()
{
	std::cout << "Usage: rivn <lua-file-name> <project-name> [<project-configuration> ...]\n";
	std::cout << "Example: rivn cmake.lua my_cmake_project\n";
	std::cout << "rivn will create a new project in the current directory with the name of <project-name>\n";
	std::cout << "rivn will then call the lua script <lua-file-name> with the project information to configure the project\n";
}


Project configureProject(int argc, char** argv) {
    Project project;

    std::vector<std::string> args(argv, argv + argc);
    if (args.size() < 3) {
        throw std::runtime_exception("Not enouth arguments");
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
                project.config[configName] = "";
                configName = projectConfiguration;
            }
            configName = projectConfiguration.substr(2);
            continue;
        }

        if (configName.empty()) {
            std::runtime_exception("Unexpected argument \"" + projectConfiguration + "\"\n");
        }
        debugPrint("\tConfiguration: \"" << configName << "\" = \"" << projectConfiguration << "\"");
        project.config[configName] = projectConfiguration;
        configName.clear();
    }

    fs::path luaFilePath = rivnPath / "lua" / (luaFileName + ".lua");

    if (!exists(luaFilePath)) {
        std::cout << "\tError: Lua file \"" << luaFilePath << "\" does not exist\n";
        return 1;
    }

    return project;
}

