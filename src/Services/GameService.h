
#pragma once

#include "../Core/Grid.h"
#include <atomic>
#include <string>
#include <memory>

// forward: rules base class
class GameRules;

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

	// Encapsulated grid accessors (UI should use these)
	int getRows() const { return grid.getR(); }
	int getCols() const { return grid.getC(); }
	bool getCell(int r, int c) const { return grid.getCell(r, c); }
	void setCell(int r, int c, bool v) { grid.setCell(r, c, v); }

	void setGridDimensions(int rows, int cols) { grid.setGridDimensions(rows, cols); buffer.setGridDimensions(rows, cols); }
	void setInitialGrid(const Grid &g) { grid = g; buffer = g; initialGrid = g; hasInitial = true; }

	// convenience: set predefined sizes
	void setGridSize(GridSize size) { grid.setGridSize(size); buffer.setGridSize(size); }

	// rule type (console UI toggles this)
	enum class RuleType { BASIC, CONWAY };
	void setRuleType(RuleType rt);
	RuleType getRuleType() const { return ruleType; }

	void setOutputBase(const std::string &b) { outputBase = b; }
	std::string getOutputBase() const { return outputBase; }

	// Input file helpers (UI calls GameService; GameService delegates to FileService)
	std::vector<std::string> listInputFiles() const;
	bool loadInitialFromFile(const std::string &path);

	// Iteration control and output
	void setIterationTarget(int it) { iterationTarget = it; }
	int getIterationTarget() const { return iterationTarget; }
	int getIterationIndex() const { return currentIteration; }

	void setTickMs(int ms) { tickMs = ms; }

private:
	Grid grid;
	Grid buffer;
	int tickMs = 200;
	std::atomic<bool> running{false};
	std::string outputBase;
	RuleType ruleType = RuleType::CONWAY;
	std::unique_ptr<GameRules> rules;
	int iterationTarget = 0; // 0 = infinite
	int currentIteration = 0;
	// Remember initial grid loaded from a file so reset() can restore it
	Grid initialGrid;
	bool hasInitial = false;
};
