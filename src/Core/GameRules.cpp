#include "GameRules.h"

static int countAliveNeighbors(const Grid &g, int r, int c) {
	int rows = g.getR();
	int cols = g.getC();
	int cnt = 0;
	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) continue;
			int nr = r + dr, nc = c + dc;
			if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) if (g.getCell(nr, nc)) ++cnt;
		}
	}
	return cnt;
}

bool ConwayRules::computeNextState(const Grid &src, int r, int c) const {
	int alive = countAliveNeighbors(src, r, c);
	bool cur = src.getCell(r, c);
	if (!cur) return (alive == 3);
	return (alive == 2 || alive == 3);
}
