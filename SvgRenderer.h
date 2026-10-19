#pragma once

#include <iosfwd>

class Canvas;
class LineShape;
class CircleShape;
class RectangleShape;

class SvgRenderer {
public:
	void write(const Canvas& canvas, int width, int height, std::ostream& output) const;

private:
    void writeHeader(int width, int height, std::ostream& output) const;
    void writeBody(const Canvas& canvas, std::ostream& output) const;
    void writeFooter(std::ostream& output) const;
    void writeLine(const LineShape& line, std::ostream& output) const;
    void writeCircle(const CircleShape& circle, std::ostream& output) const;
    void writeRectangle(const RectangleShape& rectangle, std::ostream& output) const;
};
