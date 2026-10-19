cmake_minimum_required(VERSION 3.20)

project(drawing LANGUAGES CXX)

add_executable(drawing
	main.cpp
	CLI.cpp
	Canvas.cpp
	Parser.cpp
	SvgRenderer.cpp
	PgmRenderer.cpp
)

target_compile_features(drawing PRIVATE cxx_std_20)
set_target_properties(drawing PROPERTIES CXX_EXTENSIONS OFF)
