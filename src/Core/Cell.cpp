#include "Cell.h"
#include <utility>
#include <stdexcept>

// AliveCell implementations
AliveCell::AliveCell() = default;
AliveCell::~AliveCell() = default;
bool AliveCell::isAlive() const { return true; }
void AliveCell::setAlive(bool a) { if (!a) throw std::runtime_error("Cannot set AliveCell to dead; replace with DeadCell"); }
Cell::Type AliveCell::getType() const { return Type::Normal; }
std::string AliveCell::toString() const { return "1"; }

// DeadCell implementations
DeadCell::DeadCell() = default;
DeadCell::~DeadCell() = default;
bool DeadCell::isAlive() const { return false; }
void DeadCell::setAlive(bool a) { if (a) throw std::runtime_error("Cannot set DeadCell to alive; replace with AliveCell"); }
Cell::Type DeadCell::getType() const { return Type::Normal; }
std::string DeadCell::toString() const { return "0"; }

// ObstacleCell implementations
ObstacleCell::ObstacleCell(bool alive) : alive_(alive) {}
ObstacleCell::~ObstacleCell() = default;
bool ObstacleCell::isAlive() const { return alive_; }
void ObstacleCell::setAlive(bool a) { alive_ = a; }
Cell::Type ObstacleCell::getType() const { return Type::Obstacle; }
std::string ObstacleCell::toString() const { return alive_ ? "A" : "D"; }

// Factory implementations
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
