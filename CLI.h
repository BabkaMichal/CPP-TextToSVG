#pragma once

#include <string>

struct CLIOptions {
	std::string inputFile;
	std::string outputFile;
	std::string fileExtension;
	int width{};
	int height{};
};

bool parseCLIOptions(int argc, char* argv[], CLIOptions& options, std::string& errorMessage);
