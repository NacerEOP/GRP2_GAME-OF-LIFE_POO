
#pragma once

#include "../Core/Grid.h"

// Rule and neighborhood enums used by the service interface
enum class RuleType { BASIC, CONWAY };
enum class NeighborhoodType { MOORE, VON_NEUMANN };

class GameService
{
public:
	virtual ~GameService() = default;

	// Control simulation
	virtual void start() = 0;
	virtual void pause() = 0;
	virtual void step() = 0; // advance one generation
	virtual void reset() = 0;

	// Query state
	virtual const Grid& getGrid() const = 0;
	virtual bool isRunning() const = 0;

	// Configuration
	virtual void setGridSize(GridSize size) = 0;
	virtual void setGridDimensions(int rows, int cols) = 0;
	virtual void setRuleType(RuleType rt) = 0;
	virtual RuleType getRuleType() const = 0;

	// Tick configuration
	virtual int getTickMs() const = 0;
	virtual void setTickMs(int ms) = 0;
};

// Factory to create a simple concrete GameService for terminal testing
GameService* createDefaultGameService();
