#include "Canvas.h"

#include <cmath>
#include <utility>
#define _USE_MATH_DEFINES
#include <math.h>

namespace {

//move a point by the given amount
void translatePoint(Point& point, double x, double y) {
	point.x += x;
	point.y += y;
}

//rotate a point around a center by an angle in degrees
void rotatePoint(Point& point, const Point& center, double angle) {
	constexpr double pi = M_PI;
	const double radians = angle * pi / 180.0;
	const double cosine = std::cos(radians);
	const double sine = std::sin(radians);
	const double offsetX = point.x - center.x;
	const double offsetY = point.y - center.y;

	point.x = center.x + offsetX * cosine - offsetY * sine;
	point.y = center.y + offsetX * sine + offsetY * cosine;
}

//scale a point relative to a center
void scalePoint(Point& point, const Point& center, double factor) {
	point.x = center.x + (point.x - center.x) * factor;
	point.y = center.y + (point.y - center.y) * factor;
}

} // namespace

LineShape::LineShape(const Point& start, const Point& end) : start_(start), end_(end) {}

void LineShape::translate(double x, double y) {
	translatePoint(start_, x, y);
	translatePoint(end_, x, y);
}

void LineShape::rotate(const Point& center, double angle) {
	rotatePoint(start_, center, angle);
	rotatePoint(end_, center, angle);
}

void LineShape::scale(const Point& center, double factor) {
	scalePoint(start_, center, factor);
	scalePoint(end_, center, factor);
}

const Point& LineShape::getStart() const {
	return start_;
}

const Point& LineShape::getEnd() const {
	return end_;
}

CircleShape::CircleShape(const Point& center, double radius) : center_(center), radius_(radius) {}

void CircleShape::translate(double x, double y) {
	translatePoint(center_, x, y);
}

void CircleShape::rotate(const Point& center, double angle) {
	rotatePoint(center_, center, angle);
}

void CircleShape::scale(const Point& center, double factor) {
	scalePoint(center_, center, factor);
	radius_ *= std::abs(factor);
}

const Point& CircleShape::getCenter() const {
	return center_;
}

double CircleShape::getRadius() const {
	return radius_;
}

RectangleShape::RectangleShape(const Point& topLeft, double width, double height)
	: corners_{
		topLeft,
		{topLeft.x + width, topLeft.y},
		{topLeft.x + width, topLeft.y + height},
		{topLeft.x, topLeft.y + height}
	} {}

void RectangleShape::translate(double x, double y) {
	for (Point& corner : corners_) {
		translatePoint(corner, x, y);
	}
}

void RectangleShape::rotate(const Point& center, double angle) {
	for (Point& corner : corners_) {
		rotatePoint(corner, center, angle);
	}
}

void RectangleShape::scale(const Point& center, double factor) {
	for (Point& corner : corners_) {
		scalePoint(corner, center, factor);
	}
}

const std::array<Point, 4>& RectangleShape::getCorners() const {
	return corners_;
}

void Canvas::addShape(std::unique_ptr<Shape> shape) {
	if (shape) {
		shapes_.push_back(std::move(shape));
	}
}

void Canvas::translate(double x, double y) {
	for (const std::unique_ptr<Shape>& shape : shapes_) {
		shape->translate(x, y);
	}
}

void Canvas::rotate(const Point& center, double angle) {
	for (const std::unique_ptr<Shape>& shape : shapes_) {
		shape->rotate(center, angle);
	}
}

void Canvas::scale(const Point& center, double factor) {
	for (const std::unique_ptr<Shape>& shape : shapes_) {
		shape->scale(center, factor);
	}
}

const std::vector<std::unique_ptr<Shape>>& Canvas::getShapes() const {
	return shapes_;
}
