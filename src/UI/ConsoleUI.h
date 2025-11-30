#pragma once

#include "../Services/GameService.h"

class ConsoleUI {
public:
	explicit ConsoleUI(GameService& svc);
	~ConsoleUI();

	// Main loop (blocking)
	void run();

private:
	GameService& service;

	void render() const;
	void drawHelp() const;
};

