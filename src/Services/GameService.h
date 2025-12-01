
#pragma once

#include "../Core/Grid.h"
#include <atomic>
#include <string>

class GameService {
public:
	GameService();
	~GameService();

	void start();
	void pause();
	void step();
	void reset();

	bool isRunning() const;
	int getTickMs() const { return tickMs; }

	Grid &getGrid() { return grid; }
	const Grid &getGrid() const { return grid; }

	void setGridDimensions(int rows, int cols) { grid.setGridDimensions(rows, cols); buffer.setGridDimensions(rows, cols); }
	void setInitialGrid(const Grid &g) { grid = g; buffer = g; }

	// convenience: set predefined sizes
	void setGridSize(GridSize size) { grid.setGridSize(size); buffer.setGridSize(size); }

	// rule type (console UI toggles this)
	enum class RuleType { BASIC, CONWAY };
	void setRuleType(RuleType rt) { ruleType = rt; }
	RuleType getRuleType() const { return ruleType; }

	void setOutputBase(const std::string &b) { outputBase = b; }
	std::string getOutputBase() const { return outputBase; }

	void setTickMs(int ms) { tickMs = ms; }

private:
	Grid grid;
	Grid buffer;
	int tickMs = 200;
	std::atomic<bool> running{false};
	std::string outputBase;
	RuleType ruleType = RuleType::CONWAY;
};
