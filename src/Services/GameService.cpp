
#include "GameService.h"
#include "../Core/GameRules.h"
#include "FileService.h"
#include <vector>
#include <memory>
#include <filesystem>

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
		// preserve obstacle mask from current grid into buffer so obstacles remain static
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				buffer.setObstacle(r, c, grid.isObstacle(r, c));
			}
		}
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				bool next = rules->computeNextState(grid, r, c);
				buffer.setCell(r, c, next);
			}
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
	// define some simple presets (stored in code)
	// All presets will use a 20x20 grid unless specified otherwise
	int rows = 20, cols = 20;
	grid.setGridDimensions(rows, cols);
	buffer.setGridDimensions(rows, cols);
	// clear
	for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) { grid.setCell(r,c,false); grid.setObstacle(r,c,false); }

	auto set = [&](int r, int c){ if (r>=0 && r<rows && c>=0 && c<cols) grid.setCell(r,c,true); };
	auto obs = [&](int r, int c){ if (r>=0 && r<rows && c>=0 && c<cols) grid.setObstacle(r,c,true); };

	switch (slot) {
		case 0:
			// empty
			break;
		case 1:
			// glider
			set(1,2); set(2,3); set(3,1); set(3,2); set(3,3);
			break;
		case 2:
			// blinker (oscillator)
			set(10,9); set(10,10); set(10,11);
			break;
		case 3:
			// toad
			set(9,9); set(9,10); set(9,11); set(10,8); set(10,9); set(10,10);
			break;
		case 4:
			// block (still life)
			set(5,5); set(5,6); set(6,5); set(6,6);
			break;
		case 5:
			// r-pentomino small chaos
			set(10,10); set(10,11); set(9,10); set(9,11); set(8,10);
			break;
		case 6:
			// cross
			set(10,8); set(10,9); set(10,10); set(10,11); set(10,12);
			set(8,10); set(9,10); set(11,10); set(12,10);
			break;
		case 7:
			// a few obstacles forming a wall
			for (int r = 2; r < 18; ++r) { obs(r, 5); grid.setCell(r,5,false); }
			break;
		case 8:
			// diagonal line
			for (int i=0;i<15;++i) set(i+2,i+2);
			break;
		case 9:
			// small pattern with obstacles
			set(7,7); set(7,8); set(8,7); obs(8,8); grid.setCell(8,8,false);
			break;
		default:
			break;
	}

	// apply to buffer and mark initial
	buffer = grid;
	initialGrid = grid;
	hasInitial = true;
	currentIteration = 0;
}


