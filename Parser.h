#pragma once

#include <memory>
#include <string>
#include <vector>
#include <istream>

class Command {
public:
	virtual ~Command() = default;
};

class LineCommand final : public Command {
public:
	LineCommand(int x1, int y1, int x2, int y2);
private:
	int x1_;
	int y1_;
	int x2_;
	int y2_;
};

class CircleCommand final : public Command {
public:
	CircleCommand(int x, int y, int radius);
private:
	int x_;
	int y_;
	int radius_;
};

class RectangleCommand final : public Command {
public:
	RectangleCommand(int x, int y, int width, int height);
private:
	int x_;
	int y_;
	int width_;
	int height_;
};

class TranslateCommand final : public Command {
public:
	TranslateCommand(int x, int y);
private:
	int x_;
	int y_;
};

class RotateCommand final : public Command {
public:
	RotateCommand(int x, int y, double angle);
private:
	int x_;
	int y_;
	double angle_;
};

class ScaleCommand final : public Command {
public:
	ScaleCommand(int x, int y, double factor);
private:
	int x_;
	int y_;
	double factor_;
};

bool parseInputFile(std::istream& input, std::vector<std::unique_ptr<Command>>& commands, std::string& errorMessage);
