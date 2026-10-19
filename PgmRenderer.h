#pragma once

#include <iosfwd>
#include <string>

class Canvas;
class LineShape;
class CircleShape;
class RectangleShape;

class PgmRenderer {
public:
	bool write(const Canvas& canvas, int width, int height, std::ostream& output, std::string& errorMessage) const;

private:
	class Raster;

	void writeHeader(int width, int height, std::ostream& output) const;
	void writeBody(const Canvas& canvas, Raster& raster) const;
	void writePixels(const Raster& raster, std::ostream& output) const;
	void drawLine(Raster& raster, const LineShape& line) const;
	void drawCircle(Raster& raster, const CircleShape& circle) const;
	void drawRectangle(Raster& raster, const RectangleShape& rectangle) const;
};
