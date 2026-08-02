#pragma once

#include <filesystem>
#include <string>

namespace DirectoryHelper {
	std::filesystem::path GetProjectDirectory(const std::string& proDirName);

}