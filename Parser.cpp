#include "Parser.h"

#include <charconv>
#include <cmath>
#include <sstream>
#include <utility>

LineCommand::LineCommand(int x1, int y1, int x2, int y2) : x1_(x1), y1_(y1), x2_(x2), y2_(y2) {}
CircleCommand::CircleCommand(int x, int y, int radius) : x_(x), y_(y), radius_(radius) {}
RectangleCommand::RectangleCommand(int x, int y, int width, int height) : x_(x), y_(y), width_(width), height_(height) {}
TranslateCommand::TranslateCommand(int x, int y) : x_(x), y_(y) {}
RotateCommand::RotateCommand(int x, int y, double angle) : x_(x), y_(y), angle_(angle) {}
ScaleCommand::ScaleCommand(int x, int y, double factor) : x_(x), y_(y), factor_(factor) {}

namespace {

//remove everything after #
void removeComment(std::string& line) {
	const size_t commentPosition = line.find('#');
	if (commentPosition != std::string::npos) line.erase(commentPosition);
}

//remove whitespace at the start and end
void trim(std::string& text) {
	constexpr const char* whitespace = " \t\n\r\f\v";
	const size_t first = text.find_first_not_of(whitespace);

	if (first == std::string::npos) {
		text.clear();
		return;
	}

	const size_t last = text.find_last_not_of(whitespace);
	text = text.substr(first, last - first + 1);
}

//split line into tokens
std::vector<std::string> tokenize(const std::string& line) {
	std::istringstream stream(line);
	std::vector<std::string> tokens;

	for (std::string token; stream >> token;) tokens.push_back(std::move(token));
	return tokens;
}

//str to int
bool parseInteger(const std::string& text, int& value) {
	if (text.empty()) {
		return false;
	}

	//use std::from_chars to convert the string to an integer
	int parsedValue{};
	const char* const end = text.data() + text.size();
	const auto result = std::from_chars(text.data(), end, parsedValue);

	//check for conversion errors and ensure the entire string was converted
	if (result.ec != std::errc{} || result.ptr != end) {
		return false;
	}

	value = parsedValue;
	return true;
}

//str to double
bool parseReal(const std::string& text, double& value) {
	try {
		size_t parsedLength{};
		const double parsedValue = std::stod(text, &parsedLength);

		//check conversion result and reject infinite values
		if (parsedLength != text.size() || !std::isfinite(parsedValue)) {
			return false;
		}

		value = parsedValue;
		return true;
	} catch (const std::exception&) {
		return false;
	}
}

//check if command has the expected number of tokens
bool hasTokenCount(const std::vector<std::string>& tokens, size_t count) {
	return tokens.size() == count;
}

//parse line command
std::unique_ptr<Command> parseLineCommand(const std::vector<std::string>& tokens) {
	int x1{}, y1{}, x2{}, y2{};

	if (!hasTokenCount(tokens, 5) || !parseInteger(tokens[1], x1) || !parseInteger(tokens[2], y1) ||
		!parseInteger(tokens[3], x2) || !parseInteger(tokens[4], y2) || x1 == x2 || y1 == y2) {
		return nullptr;
	}

	return std::make_unique<LineCommand>(x1, y1, x2, y2);
}

//parse circle command
std::unique_ptr<Command> parseCircleCommand(const std::vector<std::string>& tokens) {
	int x{}, y{}, radius{};

	if (!hasTokenCount(tokens, 4) || !parseInteger(tokens[1], x) || !parseInteger(tokens[2], y) ||
		!parseInteger(tokens[3], radius) || radius <= 0) {
		return nullptr;
	}

	return std::make_unique<CircleCommand>(x, y, radius);
}

//parse rectangle command
std::unique_ptr<Command> parseRectangleCommand(const std::vector<std::string>& tokens) {
	int x{}, y{}, width{}, height{};

	if (!hasTokenCount(tokens, 5) || !parseInteger(tokens[1], x) || !parseInteger(tokens[2], y) ||
		!parseInteger(tokens[3], width) || !parseInteger(tokens[4], height) || width <= 0 || height <= 0) {
		return nullptr;
	}

	return std::make_unique<RectangleCommand>(x, y, width, height);
}

//parse translate command
std::unique_ptr<Command> parseTranslateCommand(const std::vector<std::string>& tokens) {
	int x{}, y{};

	if (!hasTokenCount(tokens, 3) || !parseInteger(tokens[1], x) || !parseInteger(tokens[2], y)) {
		return nullptr;
	}

	return std::make_unique<TranslateCommand>(x, y);
}

//parse rotate command
std::unique_ptr<Command> parseRotateCommand(const std::vector<std::string>& tokens) {
	int x{}, y{};
	double angle{};

	if (!hasTokenCount(tokens, 4) || !parseInteger(tokens[1], x) || !parseInteger(tokens[2], y) || !parseReal(tokens[3], angle)) {
		return nullptr;
	}

	return std::make_unique<RotateCommand>(x, y, angle);
}

//parse scale command
std::unique_ptr<Command> parseScaleCommand(const std::vector<std::string>& tokens) {
	int x{}, y{};
	double factor{};

	if (!hasTokenCount(tokens, 4) || !parseInteger(tokens[1], x) || !parseInteger(tokens[2], y) ||
		!parseReal(tokens[3], factor) || factor == 0.0) {
		return nullptr;
	}

	return std::make_unique<ScaleCommand>(x, y, factor);
}

//choose command according to its name
std::unique_ptr<Command> parseTokens(const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		return nullptr;
	}

	const std::string& instruction = tokens.front();
	if (instruction == "line") {
		return parseLineCommand(tokens);
	}
	if (instruction == "circle") {
		return parseCircleCommand(tokens);
	}
	if (instruction == "rect") {
		return parseRectangleCommand(tokens);
	}
	if (instruction == "translate") {
		return parseTranslateCommand(tokens);
	}
	if (instruction == "rotate") {
		return parseRotateCommand(tokens);
	}
	if (instruction == "scale") {
		return parseScaleCommand(tokens);
	}

	return nullptr;
}

//tokenize line and create command
std::unique_ptr<Command> parseLine(const std::string& line) {
	return parseTokens(tokenize(line));
}

} // namespace

bool parseInputFile(std::istream& input, std::vector<std::unique_ptr<Command>>& commands, std::string& errorMessage) {
	std::string line;
	size_t lineNumber = 0;

	//read input one line at a time
	while (std::getline(input, line)) {
		lineNumber++;

		//prepare line for parsing
		removeComment(line);
		trim(line);
		if (line.empty()) {
			continue;
		}

		//create command and store it in order
		auto command = parseLine(line);
		if (!command) {
			errorMessage = "Invalid command at line "
				+ std::to_string(lineNumber)
				+ ": "
				+ line;

			return false;
		}

		commands.push_back(std::move(command));
	}

	//check if input stream failed while reading
	if (input.bad()) {
		errorMessage = "Error while reading input file.";
		return false;
	}

	return true;
}
