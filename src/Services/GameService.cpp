
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
	// Stop if we've reached the requested iteration target
	if (iterationTarget > 0 && currentIteration >= iterationTarget) {
		running = false;
		return;
	}

	int rows = grid.getR();
	int cols = grid.getC();
	// use rules polymorphically if available
	if (rules) {
		buffer.setGridDimensions(rows, cols);
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				bool next = rules->computeNextState(grid, r, c);
				buffer.setCell(r, c, next);
			}
		}
		// copy buffer back
		for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) grid.setCell(r, c, buffer.getCell(r, c));
		// write iteration output if requested and iterationTarget != 0 (disable on infinite runs)
		if (!outputBase.empty() && iterationTarget > 0) {
			++currentIteration;
			FileService::writeGridIteration(outputBase, currentIteration, grid);
			// if we reached the target, pause the simulation
			if (iterationTarget > 0 && currentIteration >= iterationTarget) {
				running = false;
			}
		} else if (!outputBase.empty() && iterationTarget == 0) {
			// infinite run: do not write iteration files to avoid disk growth
			++currentIteration; // still count iterations internally if desired
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
	grid = g;
	buffer = g;
	// remember initial loaded state
	initialGrid = g;
	hasInitial = true;
	// set output base to input file stem
	try {
		std::filesystem::path p(path);
		outputBase = p.stem().string();
	} catch (...) { outputBase = "input"; }
	currentIteration = 0;
	return true;
}


