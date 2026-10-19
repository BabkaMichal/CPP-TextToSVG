#include "PgmRenderer.h"
#include "Canvas.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <new>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace {

//clip line to image boundaries
bool clipLine(Point& start, Point& end, int width, int height) {
	const double deltaX = end.x - start.x;
	const double deltaY = end.y - start.y;
	if (!std::isfinite(deltaX) || !std::isfinite(deltaY)) {
		return false;
	}

	const double pValues[] = { -deltaX, deltaX, -deltaY, deltaY };
	const double qValues[] = {
		start.x,
		static_cast<double>(width - 1) - start.x,
		start.y,
		static_cast<double>(height - 1) - start.y
	};
	double firstParameter = 0.0;
	double lastParameter = 1.0;

	for (std::size_t index = 0; index < 4; ++index) {
		if (pValues[index] == 0.0) {
			if (qValues[index] < 0.0) {
				return false;
			}
			continue;
		}

		const double parameter = qValues[index] / pValues[index];
		if (pValues[index] < 0.0) {
			firstParameter = std::max(firstParameter, parameter);
		}
		else {
			lastParameter = std::min(lastParameter, parameter);
		}

		if (firstParameter > lastParameter) {
			return false;
		}
	}

	const Point originalStart = start;
	start.x = originalStart.x + firstParameter * deltaX;
	start.y = originalStart.y + firstParameter * deltaY;
	end.x = originalStart.x + lastParameter * deltaX;
	end.y = originalStart.y + lastParameter * deltaY;
	return true;
}

} // namespace

class PgmRenderer::Raster {
public:
	Raster(int width, int height)
		: width_(width), height_(height), pixels_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 255) {}

	//draw line using Bresenham algorithm
	void drawLine(Point start, Point end) {
		if (!clipLine(start, end, width_, height_)) {
			return;
		}

		int x = static_cast<int>(std::lround(start.x));
		int y = static_cast<int>(std::lround(start.y));
		const int targetX = static_cast<int>(std::lround(end.x));
		const int targetY = static_cast<int>(std::lround(end.y));
		const int deltaX = std::abs(targetX - x);
		const int stepX = x < targetX ? 1 : -1;
		const int deltaY = std::abs(targetY - y);
		const int stepY = y < targetY ? 1 : -1;
		long long error = static_cast<long long>(deltaX) - deltaY;

		while (true) {
			setPixel(x, y);
			if (x == targetX && y == targetY) {
				break;
			}

			const long long doubledError = 2 * error;
			if (doubledError > -deltaY) {
				error -= deltaY;
				x += stepX;
			}
			if (doubledError < deltaX) {
				error += deltaX;
				y += stepY;
			}
		}
	}

	//draw circle outline one row at a time
	void drawCircle(const Point& center, double radius) {
		if (!std::isfinite(center.x) || !std::isfinite(center.y) || !std::isfinite(radius) || radius < 0.0) {
			return;
		}

		if (radius == 0.0) {
			setRoundedPixel(center.x, center.y);
			return;
		}

		const double top = center.y - radius;
		const double bottom = center.y + radius;
		if (!std::isfinite(top) || !std::isfinite(bottom) || bottom < 0.0 || top > height_ - 1) {
			return;
		}

		const int firstY = static_cast<int>(std::max(0.0, std::ceil(top)));
		const int lastY = static_cast<int>(std::min(static_cast<double>(height_ - 1), std::floor(bottom)));
		const double radiusSquared = radius * radius;
		if (!std::isfinite(radiusSquared)) {
			return;
		}

		for (int y = firstY; y <= lastY; ++y) {
			const double offsetY = static_cast<double>(y) - center.y;
			const double remaining = radiusSquared - offsetY * offsetY;
			if (remaining < 0.0) {
				continue;
			}

			const double offsetX = std::sqrt(remaining);
			setRoundedPixel(center.x - offsetX, static_cast<double>(y));
			setRoundedPixel(center.x + offsetX, static_cast<double>(y));
		}
	}

	//draw rectangle by connecting its corners
	void drawRectangle(const std::array<Point, 4>& corners) {
		for (std::size_t index = 0; index < corners.size(); ++index) {
			const std::size_t nextIndex = (index + 1) % corners.size();
			drawLine(corners[index], corners[nextIndex]);
		}
	}

	//write pixel values as ASCII PGM
	void write(std::ostream& output) const {
		for (int y = 0; y < height_; ++y) {
			for (int x = 0; x < width_; ++x) {
				output << static_cast<int>(pixels_[pixelIndex(x, y)]);
				if ((x + 1) % 16 == 0 || x == width_ - 1) {
					output << '\n';
				}
				else {
					output << ' ';
				}
			}
		}
	}

private:
	//get pixel position in the one-dimensional array
	std::size_t pixelIndex(int x, int y) const {
		return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x);
	}

	//set pixel to black if it is inside the image
	void setPixel(int x, int y) {
		if (x >= 0 && x < width_ && y >= 0 && y < height_) {
			pixels_[pixelIndex(x, y)] = 0;
		}
	}

	//round coordinates and set the corresponding pixel
	void setRoundedPixel(double x, double y) {
		if (!std::isfinite(x) || !std::isfinite(y) ||
			x < -0.5 || x > static_cast<double>(width_) - 0.5 ||
			y < -0.5 || y > static_cast<double>(height_) - 0.5) {
			return;
		}

		setPixel(static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y)));
	}

	int width_;
	int height_;
	std::vector<unsigned char> pixels_;
};

bool PgmRenderer::write(const Canvas& canvas, int width, int height, std::ostream& output, std::string& errorMessage) const {
	//check dimensions before allocating the raster
	if (width <= 0 || height <= 0) {
		errorMessage = "Invalid PGM image dimensions.";
		return false;
	}

	//make sure the raster size does not overflow
	const std::size_t widthSize = static_cast<std::size_t>(width);
	const std::size_t heightSize = static_cast<std::size_t>(height);
	if (widthSize > std::numeric_limits<std::size_t>::max() / heightSize) {
		errorMessage = "PGM image dimensions are too large.";
		return false;
	}

	try {
		Raster raster(width, height);
		//write the PGM header, draw shapes, and write pixel values
		writeHeader(width, height, output);
		writeBody(canvas, raster);
		writePixels(raster, output);
	}
	catch (const std::bad_alloc&) {
		errorMessage = "Not enough memory to create the PGM image.";
		return false;
	}
	catch (const std::length_error&) {
		errorMessage = "PGM image dimensions are too large.";
		return false;
	}

	if (!output) {
		errorMessage = "Error while writing PGM image.";
		return false;
	}

	return true;
}

//write PGM format, image size, and maximum pixel value
void PgmRenderer::writeHeader(int width, int height, std::ostream& output) const {
	output << "P2\n" << width << " " << height << "\n255\n";
}

//draw all canvas shapes into the raster
void PgmRenderer::writeBody(const Canvas& canvas, Raster& raster) const {
	for (const auto& shape : canvas.getShapes()) {
		if (const auto* line = dynamic_cast<const LineShape*>(shape.get())) {
			drawLine(raster, *line);
		}
		else if (const auto* circle = dynamic_cast<const CircleShape*>(shape.get())) {
			drawCircle(raster, *circle);
		}
		else if (const auto* rectangle = dynamic_cast<const RectangleShape*>(shape.get())) {
			drawRectangle(raster, *rectangle);
		}
	}
}

//write raster pixel values to the output stream
void PgmRenderer::writePixels(const Raster& raster, std::ostream& output) const {
	raster.write(output);
}

//draw line shape into the raster
void PgmRenderer::drawLine(Raster& raster, const LineShape& line) const {
	raster.drawLine(line.getStart(), line.getEnd());
}

//draw circle shape into the raster
void PgmRenderer::drawCircle(Raster& raster, const CircleShape& circle) const {
	raster.drawCircle(circle.getCenter(), circle.getRadius());
}

//draw rectangle shape into the raster
void PgmRenderer::drawRectangle(Raster& raster, const RectangleShape& rectangle) const {
	raster.drawRectangle(rectangle.getCorners());
}
