
#include "GameService.h"
#include "../Core/Grid.h"

#include <vector>

class GameServiceImpl : public GameService {
public:
	GameServiceImpl()
		: grid(GridSize::NORMAL), running(false), ruleType(RuleType::CONWAY), tickMs(200) {
		// Initialize a simple glider pattern for demo
		int r = grid.getR();
		int c = grid.getC();
		int ox = 1;
		int oy = 1;
		if (r > 3 && c > 3) {
			grid.setCell(ox + 0, oy + 1, true);
			grid.setCell(ox + 1, oy + 2, true);
			grid.setCell(ox + 2, oy + 0, true);
			grid.setCell(ox + 2, oy + 1, true);
			grid.setCell(ox + 2, oy + 2, true);
		}
	}

	void start() override { running = true; }
	void pause() override { running = false; }
	void step() override {
		int rows = grid.getR();
		int cols = grid.getC();
		std::vector<std::vector<bool>> next(rows, std::vector<bool>(cols, false));

		for (int x = 0; x < rows; ++x) {
			for (int y = 0; y < cols; ++y) {
				int alive = countAliveNeighbors(x, y);
				bool current = grid.getCell(x, y);

				if (ruleType == RuleType::CONWAY) {
					if (!current) next[x][y] = (alive == 3);
					else next[x][y] = (alive == 2 || alive == 3);
				} else {
					// Basic rule as example
					if (current) next[x][y] = (alive >= 1);
					else next[x][y] = (alive >= 2);
				}
			}
		}

		// Apply next state to grid
		for (int x = 0; x < rows; ++x) {
			for (int y = 0; y < cols; ++y) {
				grid.setCell(x, y, next[x][y]);
			}
		}
	}

	void reset() override {
		// Clear grid
		grid.setGridDimensions(grid.getR(), grid.getC());
	}

	const Grid& getGrid() const override { return grid; }
	bool isRunning() const override { return running; }

	void setGridSize(GridSize size) override { grid.setGridSize(size); }
	void setGridDimensions(int rows, int cols) override { grid.setGridDimensions(rows, cols); }
	void setRuleType(RuleType rt) override { ruleType = rt; }
	RuleType getRuleType() const override { return ruleType; }

	int getTickMs() const override { return tickMs; }
	void setTickMs(int ms) override { tickMs = ms; }

private:
	Grid grid;
	bool running;
	RuleType ruleType;
	int tickMs;

	int countAliveNeighbors(int x, int y) const {
		int rows = grid.getR();
		int cols = grid.getC();
		int count = 0;

		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx == 0 && dy == 0) continue;
				int nx = x + dx;
				int ny = y + dy;
				if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
					if (grid.getCell(nx, ny)) ++count;
				}
			}
		}
		return count;
	}
};

GameService* createDefaultGameService() {
	return new GameServiceImpl();
}

