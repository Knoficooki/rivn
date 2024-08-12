#pragma once

#include <filesystem>
#include <vector>
#include <unordered_map>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <conup.hpp>

#ifdef _DEBUG
const std::string debugColor = conmod::make_fg<245, 245, 10>();
#define debugPrint(x) std::cout << debugColor << x << CONUP_RESET << '\n'
#else
#define debugPrint(x)
#endif

struct Project {
	std::string name;
	std::string generatorName;
	std::filesystem::path generatorPath;
	std::filesystem::path projectPath;
	std::unordered_map<std::string, std::string> configurations;
};

class Instance
{
public:
	enum class Command {
		UNKNOWN = -1,
		MAKE,
		CREATE,
		LIST,
	};
	
private:
	using path = std::filesystem::path;
	Instance();
	path executablePath;
	path luaModulesPath;
	path luaGeneratorsPath;

	path currentPath;

	std::vector<std::string> cmd_arguments;

	sol::state lua;

	const std::unordered_map<std::string, Command> commands = {
		{"make", Command::MAKE},
		{"create", Command::CREATE},
		{"list", Command::LIST}
	};

public:

	path getLuaModulesPath() const { return luaModulesPath; }
	path getLuaGeneratorsPath() const { return luaGeneratorsPath; }

	void printUsage() noexcept;
	
	static Instance *instance;
	static Instance& getInstance() {
		if (!instance) {
			instance = new Instance();
		}
		return *instance;
	}

	static void destroyInstance() {
		if (instance) {
			delete instance;
			instance = nullptr;
		}
	}
	
	Instance(const Instance&) = delete;
	Instance(Instance&&) = delete;
	Instance& operator=(Instance&&) = delete;
	Instance& operator=(const Instance&) = delete;

	void make(const std::vector<std::string>& arguments);
	void create(const std::vector<std::string>& arguments);
	void list(const std::vector<std::string>& arguments);
	
	~Instance() = default;

	void setup(int argc, char** argv);
};
