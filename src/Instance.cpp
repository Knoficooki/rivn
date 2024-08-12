#include "Instance.h"

Instance* Instance::instance = nullptr;

Instance::Instance()
{
	currentPath = std::filesystem::current_path();
}

const char* CommandDescriptions[] = {
	"Generate a new project",
	"Create a new template",
	"List all available generators"
};

void Instance::printUsage() noexcept
{
	static std::string usageColor = conmod::make_fg<210, 210, 180>();
	std::cout << usageColor;
	std::cout << "Usage: rivn <command>\n";
	std::cout << "\tCommands:\n";
	for (auto& [command, i] : commands)
	{
		std::cout << "\t\t" << command << " - " << CommandDescriptions[static_cast<uint32_t>(i)] << '\n';
	}
	std::cout << "\n";
	std::cout << CONUP_RESET << std::endl;
}

void Instance::make(const std::vector<std::string>& arguments)
{
	if (arguments.size() < 2) {
		throw std::runtime_error("No project name specified");
	}
	Project project;
	project.name = arguments[1];
	path luaFileName = arguments[0];
	if (luaFileName.has_extension()) {
		project.generatorName = luaFileName.filename().string();
		project.generatorPath = luaFileName;
	}
	else if (!luaFileName.has_parent_path()) {
		project.generatorName = luaFileName.string();
		project.generatorPath = luaGeneratorsPath / luaFileName.replace_extension(".lua");
	}
	else {
		throw std::runtime_error("Invalid project generator: " + luaFileName.string());
	}

	if (exists(project.projectPath)) {
		throw std::runtime_error("Project \"" + project.name + "\" already exists");
	}
}

void Instance::create(const std::vector<std::string>& arguments)
{
}

void Instance::list(const std::vector<std::string>& arguments)
{
	std::cout << "\nGenerators:\n";
	std::filesystem::directory_iterator it(luaGeneratorsPath);
	for (const auto& entry : it) {
		if (entry.is_regular_file() && entry.path().filename().string().find(".lua") != std::string::npos) {
			std::cout << " - " << conmod::make_fg<255, 255, 180>() << entry.path().filename().replace_extension().string() << CONUP_RESET << '\n';
		}
	}
}

void Instance::setup(int argc, char** argv)
{
	cmd_arguments.assign(argv, argv + argc);
	
	debugPrint("Commandline arguments:\n"; for (const auto& arg : cmd_arguments) {
		std::cout << "\t- " << arg << '\n';
	}; std::cout );
	
	executablePath = (std::filesystem::current_path() / argv[0]).parent_path().lexically_normal();
	cmd_arguments.erase(cmd_arguments.begin());
	luaModulesPath = executablePath / "modules";
	luaGeneratorsPath = executablePath / "generators";

	debugPrint("Session information:");
	debugPrint("\tCurrent directory: " << currentPath.string());
	debugPrint("\tExecutable directory: " << executablePath.string());
	debugPrint("\tLua modules directory: " << luaModulesPath.string());
	debugPrint("\tLua generators directory: " << luaGeneratorsPath.string());

	if (argc < 2) {
		throw std::runtime_error("No command specified");
	}

	std::string cmd = cmd_arguments[0];
	Command command = Command::UNKNOWN;
	try
	{
		command = commands.at(cmd);
	}
	catch (const std::out_of_range& e)
	{
		throw std::runtime_error("Unknown command \"" + cmd + "\"" + "\n\tError: " + e.what());
	}
	cmd_arguments.erase(cmd_arguments.begin());
	
	switch (command) {
	case Command::MAKE:
		this->make(cmd_arguments);
		break;
	case Command::CREATE:
		this->create(cmd_arguments);
		break;
	case Command::LIST:
		this->list(cmd_arguments);
		break;
	case Command::UNKNOWN:
	default:
		throw std::runtime_error("Command not implemented: \"" + cmd + "\"");
	}
}
