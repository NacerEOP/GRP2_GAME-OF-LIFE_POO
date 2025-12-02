#include "FileService.h"
#include "../Core/Grid.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

std::vector<std::string> FileService::listInputFiles(const std::string &inputDir) {
    std::vector<std::string> out;
    try {
        if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) return out;
        for (auto &entry : fs::directory_iterator(inputDir)) {
            if (!entry.is_regular_file()) continue;
            auto p = entry.path();
            if (p.extension() == ".txt") out.push_back(p.string());
        }
    } catch (...) {}
    return out;
}

bool FileService::readGridFromFile(const std::string &path, Grid &out) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    int rows = 0, cols = 0;
    if (!(ifs >> rows >> cols)) return false;
    if (rows <= 0 || cols <= 0) return false;
    out.setGridDimensions(rows, cols);
    std::string line;
    std::getline(ifs, line); // consume rest of first line

    for (int r = 0; r < rows; ++r) {
        if (!std::getline(ifs, line)) {
            // missing row: treat as all zeros
            for (int c = 0; c < cols; ++c) {
                out.setCell(r, c, false);
                out.setObstacle(r, c, false);
            }
            continue;
        }

        // parse tokens from the line; support 0/1 and obstacle markers A/a (alive obstacle) and D/d (dead obstacle)
        std::istringstream iss(line);
        std::string token;
        int c = 0;
        while (c < cols && (iss >> token)) {
            if (token.empty()) { ++c; continue; }
            char ch = token[0];
            if (ch == 'A' || ch == 'a') {
                out.setCell(r, c, true);
                out.setObstacle(r, c, true);
            } else if (ch == 'D' || ch == 'd') {
                out.setCell(r, c, false);
                out.setObstacle(r, c, true);
            } else {
                // try numeric
                int value = 0;
                try {
                    value = std::stoi(token);
                } catch (...) { value = 0; }
                out.setCell(r, c, value != 0);
                out.setObstacle(r, c, false);
            }
            ++c;
        }
        // pad remaining columns with zeros
        for (; c < cols; ++c) {
            out.setCell(r, c, false);
            out.setObstacle(r, c, false);
        }
    }

    return true;
}

bool FileService::writeGridIteration(const std::string &baseName, int iter, const Grid &g, const std::string &outDir) {
    try {
        fs::path od(outDir);
        if (!fs::exists(od)) fs::create_directories(od);
        std::ostringstream fname;
        fname << baseName << "_out-" << iter << ".txt";
        fs::path outPath = od / fname.str();
        std::ofstream ofs(outPath.string());
        if (!ofs.is_open()) return false;
        ofs << g.getR() << " " << g.getC() << '\n';
        for (int r = 0; r < g.getR(); ++r) {
            for (int c = 0; c < g.getC(); ++c) {
                if (g.isObstacle(r, c)) {
                    ofs << (g.getCell(r,c) ? 'A' : 'D');
                } else {
                    ofs << (g.getCell(r,c) ? 1 : 0);
                }
                if (c+1 < g.getC()) ofs << ' ';
            }
            ofs << '\n';
        }
        return true;
    } catch (...) {
        return false;
    }
}
