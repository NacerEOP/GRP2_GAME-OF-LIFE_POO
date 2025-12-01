
#include "GameService.h"
#include "../Core/GameRules.h"
#include <vector>
#include <memory>

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
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				bool next = rules->computeNextState(grid, r, c);
				buffer.setCell(r, c, next);
			}
		}
		// copy buffer back
		for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) grid.setCell(r, c, buffer.getCell(r, c));
	} else {
		// fallback: do nothing
	}
}

void GameService::reset() {
	grid.setGridDimensions(20,20);
	buffer.setGridDimensions(20,20);
}


