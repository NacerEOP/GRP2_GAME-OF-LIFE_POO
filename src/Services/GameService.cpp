
#include "GameService.h"
#include "../Core/GameRules.h"
#include "FileService.h"
#include <vector>
#include <memory>
#include <filesystem>
#include <thread>
#include <algorithm>

GameService::GameService() {
	// default 20x20
	grid.setGridDimensions(20,20);
	buffer.setGridDimensions(20,20);
	// default rule
	rules = std::make_unique<ConwayRules>();
}

GameService::~GameService() {}

void GameService::start() { running = true; }
void GameService::pause() { running = false; }
bool GameService::isRunning() const { return running.load(); }

void GameService::setRuleType(RuleType rt) {
	ruleType = rt;
	if (rt == RuleType::CONWAY) rules = std::make_unique<ConwayRules>();
	else rules.reset(); // BASIC not implemented yet -> nullptr (fallback)
}

void GameService::step() {
	int rows = grid.getR();
	int cols = grid.getC();
	// use rules polymorphically if available
	if (rules) {
		buffer.setGridDimensions(rows, cols);

		// Parallel compute next state per row range. Each worker will also copy
		// obstacle flag for its rows into the buffer to avoid a separate pass.
		unsigned int hw = std::thread::hardware_concurrency();
		unsigned int nThreads = hw == 0 ? 2u : hw;
		if ((int)nThreads > rows) nThreads = rows > 0 ? (unsigned int)rows : 1u;

		std::vector<std::thread> workers;
		workers.reserve(nThreads);
		int chunk = rows / (int)nThreads;
		int rem = rows % (int)nThreads;
		int start = 0;
		for (unsigned int t = 0; t < nThreads; ++t) {
			int add = (t < (unsigned int)rem) ? 1 : 0;
			int end = start + chunk + add;
			workers.emplace_back([this, start, end, cols]() {
				for (int r = start; r < end; ++r) {
					for (int c = 0; c < cols; ++c) {
						// copy obstacle flag for this cell
						buffer.setObstacle(r, c, grid.isObstacle(r, c));
						bool next = rules->computeNextState(grid, r, c);
						buffer.setCell(r, c, next);
					}
				}
			});
			start = end;
		}
		for (auto &th : workers) if (th.joinable()) th.join();
		// detect stabilization: if buffer equals grid, stop running
		if (buffer.equals(grid)) {
			running = false;
			// still increment iteration counter to indicate a step attempt
			++currentIteration;
			return;
		}
		// copy buffer back
		for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) grid.setCell(r, c, buffer.getCell(r, c));
		// increment iteration counter
		++currentIteration;
		// decide whether to write this iteration to file: save only first `saveIterations` when >0
		if (!outputBase.empty() && saveIterations > 0 && currentIteration <= saveIterations) {
			FileService::writeGridIteration(outputBase, currentIteration, grid);
		}
	} else {
		// fallback: do nothing
	}
}

void GameService::reset() {
	if (hasInitial) {
		grid = initialGrid;
		buffer = initialGrid;
	} else {
		grid.setGridDimensions(20,20);
		buffer.setGridDimensions(20,20);
	}
	// reset iteration counter
	currentIteration = 0;
}

std::vector<std::string> GameService::listInputFiles() const {
	return FileService::listInputFiles("Input");
}

bool GameService::loadInitialFromFile(const std::string &path) {
	Grid g;
	if (!FileService::readGridFromFile(path, g)) return false;
	// preserve current toric setting so loading a file doesn't disable wrapping
	bool wasToric = grid.isToric();
	grid = g;
	buffer = g;
	grid.setToric(wasToric);
	buffer.setToric(wasToric);
	// remember initial loaded state
	initialGrid = g;
	initialGrid.setToric(wasToric);
	hasInitial = true;
	// set output base to input file stem
	try {
		std::filesystem::path p(path);
		outputBase = p.stem().string();
	} catch (...) { outputBase = "input"; }
	currentIteration = 0;
	return true;
}

void GameService::loadPreset(int slot) {
	// helper to clear a grid of given size
	auto prepare = [&](int rows, int cols) {
		grid.setGridDimensions(rows, cols);
		buffer.setGridDimensions(rows, cols);
		for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) { grid.setCell(r,c,false); grid.setObstacle(r,c,false); }
	};

	// helper to place a set of relative coordinates centered in the grid
	auto placePattern = [&](const std::vector<std::pair<int,int>> &coords, int patternH, int patternW) {
		int rows = grid.getR();
		int cols = grid.getC();
		int offR = (rows - patternH) / 2;
		int offC = (cols - patternW) / 2;
		for (auto &p : coords) {
			int rr = offR + p.first;
			int cc = offC + p.second;
			if (rr >= 0 && rr < rows && cc >= 0 && cc < cols) grid.setCell(rr, cc, true);
		}
	};

	switch (slot) {
		case 0: {
			// empty 20x20
			prepare(20,20);
		} break;
		case 1: {
			// glider (20x20)
			prepare(20,20);
			auto coords = std::vector<std::pair<int,int>>{{1,2},{2,3},{3,1},{3,2},{3,3}};
			placePattern(coords, 5,5);
		} break;
		case 2: {
			// blinker (20x20)
			prepare(20,20);
			auto coords = std::vector<std::pair<int,int>>{{10,9},{10,10},{10,11}};
			placePattern(coords,3,13);
		} break;
		case 3: {
			// toad (20x20)
			prepare(20,20);
			auto coords = std::vector<std::pair<int,int>>{{9,9},{9,10},{9,11},{10,8},{10,9},{10,10}};
			placePattern(coords,4,6);
		} break;
		case 4: {
			// block (still life)
			prepare(20,20);
			auto coords = std::vector<std::pair<int,int>>{{5,5},{5,6},{6,5},{6,6}};
			placePattern(coords,2,2);
		} break;
		case 5: {
			// r-pentomino (small chaos)
			prepare(20,20);
			auto coords = std::vector<std::pair<int,int>>{{10,10},{10,11},{9,10},{9,11},{8,10}};
			placePattern(coords,3,4);
		} break;
		case 6: {
			// cross
			prepare(20,20);
			auto coords = std::vector<std::pair<int,int>>{{10,8},{10,9},{10,10},{10,11},{10,12},{8,10},{9,10},{11,10},{12,10}};
			placePattern(coords,5,5);
		} break;
		case 7: {
			// obstacles wall
			prepare(20,20);
			for (int r = 2; r < 18; ++r) { grid.setObstacle(r, 5, true); grid.setCell(r,5,false); }
		} break;
		case 8: {
			// diagonal line
			prepare(20,20);
			for (int i=0;i<15;++i) grid.setCell(i+2,i+2,true);
		} break;
		case 9: {
			// small pattern with obstacle
			prepare(20,20);
			grid.setCell(7,7,true); grid.setCell(7,8,true); grid.setCell(8,7,true);
			grid.setObstacle(8,8,true); grid.setCell(8,8,false);
		} break;
		case 10: {
			// Gosper glider gun (approx 9x36) - center in a 20x38 grid
			prepare(20,38);
			std::vector<std::pair<int,int>> coords = {
				{5,1},{5,2},{6,1},{6,2},
				{5,11},{6,11},{7,11},{4,12},{8,12},{3,13},{9,13},{3,14},{9,14},{6,15},{4,16},{8,16},{5,17},{6,17},{7,17},{6,18},
				{3,21},{4,21},{5,21},{3,22},{4,22},{5,22},{2,23},{6,23},{1,25},{2,25},{6,25},{7,25},
				{3,35},{4,35},{3,36},{4,36}
			};
			placePattern(coords,20,38); // coords already use rows within 0..19
		} break;
		case 11: {
			// Pulsar (centered) - use 17x17 grid
			prepare(17,17);
			std::vector<std::pair<int,int>> coords = {
				{2,4},{2,5},{2,6},{2,10},{2,11},{2,12},
				{4,2},{5,2},{6,2},{4,7},{5,7},{6,7},{4,9},{5,9},{6,9},{4,14},{5,14},{6,14},
				{7,4},{7,5},{7,6},{7,10},{7,11},{7,12},
				{9,4},{9,5},{9,6},{9,10},{9,11},{9,12},
				{10,2},{11,2},{12,2},{10,7},{11,7},{12,7},{10,9},{11,9},{12,9},{10,14},{11,14},{12,14},
				{14,4},{14,5},{14,6},{14,10},{14,11},{14,12}
			};
			placePattern(coords,17,17);
		} break;
		case 12: {
			// Lightweight spaceship (LWSS)
			prepare(10,15);
			std::vector<std::pair<int,int>> coords = {{1,1},{1,4},{2,0},{2,1},{2,2},{2,3},{2,4},{3,0},{3,4},{4,0},{4,3}};
			placePattern(coords,5,6);
		} break;
		case 13: {
			// Diehard (small, destructive) - needs small area
			prepare(10,20);
			std::vector<std::pair<int,int>> coords = {{4,1},{4,2},{4,6},{3,6},{4,7},{4,8},{4,9}}; // rough layout
			placePattern(coords,6,12);
		} break;
		default: {
			// unknown -> empty 20x20
			prepare(20,20);
		} break;
	}

	// apply to buffer and mark initial
	buffer = grid;
	initialGrid = grid;
	hasInitial = true;
	currentIteration = 0;
}


