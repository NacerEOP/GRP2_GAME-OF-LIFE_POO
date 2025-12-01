#pragma once

#include <string>
#include <vector>

class Grid;

namespace FileService {
    // List .txt files in the `Input` directory (relative to working dir)
    std::vector<std::string> listInputFiles(const std::string &inputDir = "Input");

    // Read grid file. Returns true on success and fills `out` grid.
    bool readGridFromFile(const std::string &path, Grid &out);

    // Write a grid iteration to Output/<base>_out-<iter>.txt
    bool writeGridIteration(const std::string &baseName, int iter, const Grid &g, const std::string &outDir = "Output");
}
