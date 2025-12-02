#include "Cell.h"
#include <utility>
#include <stdexcept>

// Concrete AliveCell
class AliveCell : public Cell {
public:
	AliveCell() = default;
	~AliveCell() override = default;
	bool isAlive() const override { return true; }
	void setAlive(bool a) override { if (!a) throw std::runtime_error("Cannot set AliveCell to dead; replace with DeadCell"); }
	Type getType() const override { return Type::Normal; }
	std::string toString() const override { return "1"; }
};

// Concrete DeadCell
class DeadCell : public Cell {
public:
	DeadCell() = default;
	~DeadCell() override = default;
	bool isAlive() const override { return false; }
	void setAlive(bool a) override { if (a) throw std::runtime_error("Cannot set DeadCell to alive; replace with AliveCell"); }
	Type getType() const override { return Type::Normal; }
	std::string toString() const override { return "0"; }
};

// Concrete ObstacleCell (can be alive or dead but is immutable under rules)
class ObstacleCell : public Cell {
public:
	ObstacleCell(bool alive) : alive_(alive) {}
	~ObstacleCell() override = default;
	bool isAlive() const override { return alive_; }
	void setAlive(bool a) override { alive_ = a; }
	Type getType() const override { return Type::Obstacle; }
	std::string toString() const override { return alive_ ? "A" : "D"; }
private:
	bool alive_;
};

std::unique_ptr<Cell> Cell::createDefault(bool alive, Type t) {
	if (t == Type::Obstacle) {
		return std::make_unique<ObstacleCell>(alive);
	}
	if (alive) return std::make_unique<AliveCell>();
	return std::make_unique<DeadCell>();
}

std::unique_ptr<Cell> Cell::createAlive() { return std::make_unique<AliveCell>(); }
std::unique_ptr<Cell> Cell::createDead() { return std::make_unique<DeadCell>(); }
std::unique_ptr<Cell> Cell::createObstacle(bool alive) { return std::make_unique<ObstacleCell>(alive); }
