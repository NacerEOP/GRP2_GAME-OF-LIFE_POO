
#pragma once

#include <string>
#include <vector>

#include "../Core/Grid.h"

class FileService {
public:
	// List input files (txt) in a directory
	static std::vector<std::string> listInputFiles(const std::string &inputDir);

	// Read grid from file into provided Grid reference
	// Format: first line: <rows> <cols>
	// Following lines: tokens per cell (0/1 or A/D for obstacle alive/dead)
	static bool readGridFromFile(const std::string &path, Grid &out);

	// Write a grid iteration to an output directory with a base name
	static bool writeGridIteration(const std::string &baseName, int iter, const Grid &g, const std::string &outDir = "Output");
};


