# GRP2_GAME-OF-LIFE_POO
This repository contains a c++ project that is the game of life


/* 
Hello mister yanis on doit faire le code
hello mister nacer benkherouf tu es le meilleur dans le monde 
Hello everyone, I AGREE 
*/




commande pour compiler le mode Console:


g++ -std=c++17 -Iinclude -Isrc `
 src/main.cpp src/UI/ConsoleUI.cpp src/Core/GameRules.cpp src/Core/Grid.cpp src/Core/Cell.cpp src/Services/GameService.cpp `
 -o TestConsole.exe