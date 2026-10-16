#include "CLI.h"

#include <charconv>
#include <system_error>

namespace {

//digit check
bool digitOnly(const std::string& text) {
	if (text.empty()) return false;
	for (const char character : text) {
		if (character < '0' || character > '9') return false;
	}
	return true;
}

//str to int
bool parsePositiveInteger(const std::string& text, int& value) {
	//contains string or empty check
	if (!digitOnly(text)) {
		return false;
	}

	//use std::from_chars to convert the string to an integer
	int parsedValue{};
	const char* end = text.data() + text.size();
	const auto result = std::from_chars(text.data(), end, parsedValue);

	//check for conversion errors, ensure the entire string was converted and if its positive
	if (result.ec != std::errc{} || result.ptr != end || parsedValue <= 0) {
		return false;
	}

	value = parsedValue;
	return true;
}

//get width and height of drawing board
bool parseResolution(const std::string& resolution, int& width, int& height) {
	const size_t xPos = resolution.find('x');

	//check if x is found and if there is only one x in the string
	if (xPos == std::string::npos || resolution.find('x', xPos + 1) != std::string::npos) {
		return false;
	}

	int parsedWidth{};
	int parsedHeight{};

	//parse width and height from the resolution string
	if (!parsePositiveInteger(resolution.substr(0, xPos), parsedWidth) ||
		!parsePositiveInteger(resolution.substr(xPos + 1), parsedHeight)) {
		return false;
	}

	width = parsedWidth;
	height = parsedHeight;
	return true;
}


bool getSupportedExtension(const std::string& filename, std::string& extension) {
	//find last position of dot in string
	const size_t dotPosition = filename.find_last_of('.');
	if (dotPosition == std::string::npos) {
		return false;
	}

	//get extension
	const std::string parsedExtension = filename.substr(dotPosition + 1);


	if (parsedExtension != "pgm" && parsedExtension != "svg") {
		return false;
	}

	extension = parsedExtension;
	return true;
}

}//namespace

bool parseCLIOptions(int argc, char* argv[], CLIOptions& options, std::string& errorMessage) {
	if (argc != 4) {
		errorMessage = "Invalid number of arguments. Usage: drawing.exe <input_file> <output_file> <size>";
		return false;
	}

	int width{};
	int height{};
	if (!parseResolution(argv[3], width, height)) {
		errorMessage = "Invalid resolution format. Please use positive integers in the format <width>x<height>.";
		return false;
	}

	std::string extension;
	if (!getSupportedExtension(argv[2], extension)) {
		errorMessage = "Invalid output file. Please use a .pgm or .svg file.";
		return false;
	}

	options.inputFile = argv[1];
	options.outputFile = argv[2];
	options.fileExtension = extension;
	options.width = width;
	options.height = height;
	return true;
}
