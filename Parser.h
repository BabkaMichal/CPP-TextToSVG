#pragma once

#include <memory>
#include <string>
#include <vector>
#include <istream>

class Canvas;

class Command {
public:
	virtual ~Command() = default;
	virtual void execute(Canvas& canvas) const = 0;
};

class LineCommand final : public Command {
public:
	LineCommand(int x1, int y1, int x2, int y2);
	void execute(Canvas& canvas) const override;
private:
	int x1_;
	int y1_;
	int x2_;
	int y2_;
};

class CircleCommand final : public Command {
public:
	CircleCommand(int x, int y, int radius);
	void execute(Canvas& canvas) const override;
private:
	int x_;
	int y_;
	int radius_;
};

class RectangleCommand final : public Command {
public:
	RectangleCommand(int x, int y, int width, int height);
	void execute(Canvas& canvas) const override;
private:
	int x_;
	int y_;
	int width_;
	int height_;
};

class TranslateCommand final : public Command {
public:
	TranslateCommand(int x, int y);
	void execute(Canvas& canvas) const override;
private:
	int x_;
	int y_;
};

class RotateCommand final : public Command {
public:
	RotateCommand(int x, int y, double angle);
	void execute(Canvas& canvas) const override;
private:
	int x_;
	int y_;
	double angle_;
};

class ScaleCommand final : public Command {
public:
	ScaleCommand(int x, int y, double factor);
	void execute(Canvas& canvas) const override;
private:
	int x_;
	int y_;
	double factor_;
};

bool parseInputFile(std::istream& input, std::vector<std::unique_ptr<Command>>& commands, std::string& errorMessage);
