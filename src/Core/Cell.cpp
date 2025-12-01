#include "Cell.h"
#include <utility>

// Concrete default implementation of Cell
class DefaultCell : public Cell {
public:
	DefaultCell(bool alive, Type t) : alive_(alive), type_(t) {}
	~DefaultCell() override = default;

	bool isAlive() const override { return alive_; }
	void setAlive(bool a) override { if (type_ != Type::Obstacle) alive_ = a; }
	Type getType() const override { return type_; }
	void setType(Type t) override { type_ = t; }
	std::string toString() const override { return isAlive() ? "1" : "0"; }

private:
	bool alive_;
	Type type_;
};

std::unique_ptr<Cell> Cell::createDefault(bool alive, Type t) {
	return std::make_unique<DefaultCell>(alive, t);
}
