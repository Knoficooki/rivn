#pragma once

#include <sol/sol.hpp>
#include <filesystem>
#include <unordered_map>


#include "../Instance.h"

class Generator
{
	sol::state& lua;
	std::filesystem::path generatorPath;
public:
	Generator(sol::state& lua, const std::string& generatorName) : lua(lua) {
		if (generatorName.empty()) {
			throw std::runtime_error("Generator name cannot be empty");
		}

		if (std::filesystem::path(generatorName).has_extension()) {
			generatorPath = generatorName;
		} else {
			generatorPath = Instance::getInstance().getLuaGeneratorsPath() / generatorName;
		}

		if (!exists(generatorPath)) {
			throw std::runtime_error("Generator \"" + generatorName + "\" does not exist");
		}
	}

	int call(const std::unordered_map<std::string, std::string>& configurations);
};
