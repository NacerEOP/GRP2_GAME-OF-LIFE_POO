
#include "GameService.h"
#include <vector>
#include <iostream>

GameService::GameService() {
	// default 20x20
	grid.setGridDimensions(20,20);
	buffer.setGridDimensions(20,20);
}

GameService::~GameService() {}

void GameService::start() { running = true; }
void GameService::pause() { running = false; }
bool GameService::isRunning() const { return running.load(); }

static int countAliveNeighbors(const Grid &g, int x, int y) {
	int rows = g.getR();
	int cols = g.getC();
	int cnt = 0;
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			int nx = x + dx;
			int ny = y + dy;
			if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
				if (g.getCell(nx, ny)) ++cnt;
			}
		}
	}
	return cnt;
}

void GameService::step() {
	int rows = grid.getR();
	int cols = grid.getC();
	std::vector<std::vector<bool>> next(rows, std::vector<bool>(cols, false));

	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			int alive = countAliveNeighbors(grid, r, c);
			bool cur = grid.getCell(r, c);
			if (ruleType == GameService::RuleType::CONWAY) {
				if (!cur) next[r][c] = (alive == 3);
				else next[r][c] = (alive == 2 || alive == 3);
			} else {
				// simple alternative rule
				if (!cur) next[r][c] = (alive >= 2);
				else next[r][c] = (alive >= 1);
			}
		}
	}

	// copy back to grid
	for (int r = 0; r < rows; ++r) for (int c = 0; c < cols; ++c) grid.setCell(r, c, next[r][c]);

	// TODO: iteration output (FileService) not implemented in this commit
}

void GameService::reset() {
	grid.setGridDimensions(20,20);
	buffer.setGridDimensions(20,20);
}

// expose rule-type setters/getters already implemented inline in header; nothing else needed here


