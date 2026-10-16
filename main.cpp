#include "CLI.h"
#include "Parser.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
	CLIOptions options;
	std::string errorMessage;

	//parsing and validating CLI arguments
	if (!parseCLIOptions(argc, argv, options, errorMessage)) {
		std::cerr << errorMessage << std::endl;
		return 1;
	}

	//opening file
	std::ifstream input(options.inputFile);
	if (!input.is_open()) {
		std::cerr << "Error: Unable to open input file: " << options.inputFile << std::endl;
		return 1;
	}

	//parsing commands from file
	std::vector<std::unique_ptr<Command>> commands;
	if (!parseInputFile(input, commands, errorMessage)) {
		std::cerr << errorMessage << std::endl;
		return 1;
	}

	//print success message and number of parsed commands
	std::cout << "OK" << std::endl << commands.size() << std::endl;
	return 0;

	//TODO make canvas
}
