#pragma once

#include <array>
#include <memory>
#include <vector>

struct Point {
	double x{};
	double y{};
};

class Shape {
public:
	virtual ~Shape() = default;

	virtual void translate(double x, double y) = 0;
	virtual void rotate(const Point& center, double angle) = 0;
	virtual void scale(const Point& center, double factor) = 0;
};

class LineShape final : public Shape {
public:
	LineShape(const Point& start, const Point& end);

	void translate(double x, double y) override;
	void rotate(const Point& center, double angle) override;
	void scale(const Point& center, double factor) override;

	const Point& getStart() const;
	const Point& getEnd() const;

private:
	Point start_;
	Point end_;
};

class CircleShape final : public Shape {
public:
	CircleShape(const Point& center, double radius);

	void translate(double x, double y) override;
	void rotate(const Point& center, double angle) override;
	void scale(const Point& center, double factor) override;

	const Point& getCenter() const;
	double getRadius() const;

private:
	Point center_;
	double radius_{};
};

class RectangleShape final : public Shape {
public:
	RectangleShape(const Point& topLeft, double width, double height);

	void translate(double x, double y) override;
	void rotate(const Point& center, double angle) override;
	void scale(const Point& center, double factor) override;

	const std::array<Point, 4>& getCorners() const;

private:
	//store all corners because a rotated rectangle may not stay axis-aligned
	std::array<Point, 4> corners_;
};

class Canvas {
public:
	void addShape(std::unique_ptr<Shape> shape);

	void translate(double x, double y);
	void rotate(const Point& center, double angle);
	void scale(const Point& center, double factor);

	const std::vector<std::unique_ptr<Shape>>& getShapes() const;

private:
	std::vector<std::unique_ptr<Shape>> shapes_;
};
