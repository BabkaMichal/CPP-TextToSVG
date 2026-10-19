#include "CLI.h"
#include "Parser.h"
#include "Canvas.h"
#include "SvgRenderer.h"
#include "PgmRenderer.h"

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

	//open input file
	std::ifstream input(options.inputFile);
	if (!input.is_open()) {
		std::cerr << "Error: Unable to open input file: " << options.inputFile << std::endl;
		return 1;
	}

	//parse commands from input file
	std::vector<std::unique_ptr<Command>> commands;
	if (!parseInputFile(input, commands, errorMessage)) {
		std::cerr << errorMessage << std::endl;
		return 1;
	}

	//execute commands in order and store the resulting shapes in canvas
	Canvas canvas;
	for (const auto& command : commands) {
		command->execute(canvas);
	}

	//open output file
	std::ofstream output(options.outputFile);
	if (!output.is_open()) {
		std::cerr << "Error: Unable to open output file: " << options.outputFile << std::endl;
		return 1;
	}

	//select renderer according to output file extension
	if (options.fileExtension == "svg") {
		SvgRenderer renderer;
		renderer.write(canvas, options.width, options.height, output);
	}
	else if (options.fileExtension == "pgm") {
		PgmRenderer renderer;
		if (!renderer.write(canvas, options.width, options.height, output, errorMessage)) {
			std::cerr << errorMessage << std::endl;
			return 1;
		}
	}

	//close file
	output.close();
	if (!output) {
		std::cerr << "Error while writing output file: " << options.outputFile << std::endl;
		return 1;
	}

	//print success message and number of parsed commands
	std::cout << "OK" << std::endl << commands.size() << std::endl;
	return 0;
}
