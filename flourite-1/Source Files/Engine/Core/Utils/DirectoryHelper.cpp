// DirectoryHelper.cpp

#include <filesystem>
#include <windows.h>
#include <iostream>
#include "Engine/Core/Utils/DirectoryHelper.h"

std::filesystem::path DirectoryHelper::GetProjectDirectory(const std::string& proDirName) {
	TCHAR buffer[MAX_PATH];

	if (GetModuleFileName(NULL, buffer, MAX_PATH) == 0) {
		std::cerr << "[DirectoryHelper Error]: GetModuleName Failed" << std::endl;
		
		return std::filesystem::path{};
	}

	std::filesystem::path exeFullPath(buffer);

	std::cout << exeFullPath.parent_path().parent_path().parent_path() / proDirName << std::endl;

	// 프로젝트 디렉토리는 "../../../{proDirName}"
	return exeFullPath.parent_path().parent_path().parent_path() / proDirName;
}