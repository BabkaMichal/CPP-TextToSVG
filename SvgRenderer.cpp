#include "SvgRenderer.h"
#include "Canvas.h"

#include <cstddef>
#include <ostream>
#include <vector>

void SvgRenderer::write(const Canvas& canvas, int width, int height, std::ostream& output) const {
	writeHeader(width, height, output);
	writeBody(canvas, output);
	writeFooter(output);
}

void SvgRenderer::writeHeader(int width, int height, std::ostream& output) const {
	output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	output << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
		<< "width=\"" << width << "\" "
		<< "height=\"" << height << "\" "
		<< "viewBox=\"0 0 " << width << " " << height << "\" "
		<< "style=\"background-color: #FFFFFF\">\n";
}

void SvgRenderer::writeBody(const Canvas& canvas, std::ostream& output) const {
	//check the type of each shape and call its write function
	for (const auto& shape : canvas.getShapes()) {
		if (const auto* line = dynamic_cast<const LineShape*>(shape.get())) {
			writeLine(*line, output);
		}
		else if (const auto* circle = dynamic_cast<const CircleShape*>(shape.get())) {
			writeCircle(*circle, output);
		}
		else if (const auto* rectangle = dynamic_cast<const RectangleShape*>(shape.get())) {
			writeRectangle(*rectangle, output);
		}
	}
}

void SvgRenderer::writeFooter(std::ostream& output) const {
	output << "</svg>\n";
}

void SvgRenderer::writeLine(const LineShape& line, std::ostream& output) const {
	const Point& start = line.getStart();
	const Point& end = line.getEnd();

	output << "<line "
		<< "x1=\"" << start.x << "\" "
		<< "y1=\"" << start.y << "\" "
		<< "x2=\"" << end.x << "\" "
		<< "y2=\"" << end.y << "\" "
		<< "stroke=\"#000000\" "
		<< "stroke-width=\"1\" />\n";
}

void SvgRenderer::writeCircle(const CircleShape& circle, std::ostream& output) const {
	const Point& center = circle.getCenter();
	const double radius = circle.getRadius();

	output << "<circle "
		<< "cx=\"" << center.x << "\" "
		<< "cy=\"" << center.y << "\" "
		<< "r=\"" << radius << "\" "
		<< "stroke=\"#000000\" "
		<< "fill=\"none\" "
		<< "stroke-width=\"1\" />\n";
}

void SvgRenderer::writeRectangle(const RectangleShape& rectangle, std::ostream& output) const {
	const std::array<Point, 4>& corners = rectangle.getCorners();

	output << "<polygon points=\"";
	for (std::size_t index = 0; index < corners.size(); ++index) {
		if (index > 0) {
			output << " ";
		}

		output << corners[index].x << "," << corners[index].y;
	}

	output << "\" "
		<< "stroke=\"#000000\" "
		<< "fill=\"none\" "
		<< "stroke-width=\"1\" />\n";
}
