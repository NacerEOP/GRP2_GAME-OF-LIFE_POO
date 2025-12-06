#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <chrono>
#include <iostream>
#include <fstream>
#include "../src/Services/GameService.h"
#include "../src/Services/FileService.h"

static std::string gridToString(const Grid &g) {
    std::ostringstream oss;
    for (int i = 0; i < g.getR(); ++i) {
        for (int j = 0; j < g.getC(); ++j) {
            if (g.isObstacle(i,j)) oss << 'A';
            else oss << (g.getCell(i,j) ? '1' : '0');
        }
        if (i < g.getR()-1) oss << '\n';
    }
    return oss.str();
}

static void dumpExpectedActual(const Grid &expected, const Grid &actual) {
    std::cout << "EXPECTED:\n" << gridToString(expected) << "\n";
    std::cout << "ACTUAL:\n" << gridToString(actual) << "\n";
}


TEST_CASE("blinker", "[evolution]") {
    auto t0 = std::chrono::steady_clock::now();
    std::cout << "[DEBUT] clignotant\n";
    GameService svc;
    svc.setGridDimensions(5,5);
    svc.setToric(false);
    svc.setTickMs(10);

    svc.setCell(2,1,true);
    svc.setCell(2,2,true);
    svc.setCell(2,3,true);

    svc.step();
    Grid actual1 = svc.getGrid();
    Grid expected1;
    expected1.setGridDimensions(5,5);
    expected1.setCell(1,2,true);
    expected1.setCell(1,2,true);
    expected1.setCell(2,2,true);
    expected1.setCell(3,2,true);
    // Always show expected vs actual for easier debugging
    std::cout << "Clignotant - etape 1 (attendu vs obtenu):\n";
    dumpExpectedActual(expected1, actual1);
    REQUIRE( expected1.equals(actual1) );
    svc.step();
    Grid actual2 = svc.getGrid();
    Grid expected2;
    expected2.setGridDimensions(5,5);
    expected2.setCell(2,1,true);
    expected2.setCell(2,2,true);
    expected2.setCell(2,3,true);
    std::cout << "Clignotant - etape 2 (attendu vs obtenu):\n";
    dumpExpectedActual(expected2, actual2);
    REQUIRE( expected2.equals(actual2) );

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[FIN] clignotant (" << ms << " ms)\n";
}

TEST_CASE("block_stable", "[evolution]") {
    auto t0 = std::chrono::steady_clock::now();
    std::cout << "[DEBUT] bloc_stable\n";
    GameService svc;
    svc.setGridDimensions(4,4);
    svc.setToric(false);
    svc.setCell(1,1,true);
    svc.setCell(1,2,true);
    svc.setCell(2,1,true);
    svc.setCell(2,2,true);

    Grid before = svc.getGrid();
    svc.step();
    Grid after = svc.getGrid();
    // Always dump expected (before) vs actual (after)
    std::cout << "Bloc stable (attendu vs obtenu):\n";
    dumpExpectedActual(before, after);
    REQUIRE( before.equals(after) );

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[FIN] bloc_stable (" << ms << " ms)\n";
}

TEST_CASE("toric_wrap", "[toric]") {
    auto t0 = std::chrono::steady_clock::now();
    std::cout << "[DEBUT] toric_wrap\n";
    GameService svc;
    svc.setGridDimensions(3,3);
    svc.setToric(true);
    svc.setCell(2,2,true);
    int r = svc.getRows();
    int c = svc.getCols();
    int wx = ((-1 % r) + r) % r;
    int wy = ((-1 % c) + c) % c;
    // Always dump expected vs actual for toric wrap
    Grid expectedToric;
    expectedToric.setGridDimensions(3,3);
    expectedToric.setCell(2,2,true);
    std::cout << "Toric wrap (attendu vs obtenu):\n";
    dumpExpectedActual(expectedToric, svc.getGrid());
    REQUIRE( svc.getGrid().getCell(wx,wy) == true );

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[FIN] toric_wrap (" << ms << " ms)\n";
}

TEST_CASE("obstacle_preserved", "[obstacle]") {
    auto t0 = std::chrono::steady_clock::now();
    std::cout << "[DEBUT] obstacle_preserved\n";
    GameService svc;
    svc.setGridDimensions(5,5);
    svc.setToric(false);
    svc.setObstacle(2,2,true);
    svc.setCell(1,2,true);
    svc.setCell(2,1,true);
    svc.setCell(2,3,true);

    bool before = svc.getCell(2,2);
    svc.step();
    bool after = svc.getCell(2,2);
    // Always show expected vs actual for obstacle preservation
    Grid expectedObs = svc.getGrid();
    expectedObs.setCell(2,2,before);
    std::cout << "Obstacle preserved (attendu vs obtenu):\n";
    dumpExpectedActual(expectedObs, svc.getGrid());
    REQUIRE( before == after );

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[FIN] obstacle_preserved (" << ms << " ms)\n";
}

TEST_CASE("file_io", "[io]") {
    auto t0 = std::chrono::steady_clock::now();
    std::cout << "[DEBUT] entree_sortie_fichier\n";
    const char *path = "tests/sample_input.txt";
    std::ofstream ofs(path);
    ofs << "3 3\n";
    ofs << "1 0 1\n";
    ofs << "0 A 0\n";
    ofs << "1 0 0\n";
    ofs.close();

    Grid g;
    bool ok = FileService::readGridFromFile(path, g);
        if (!ok) {
            std::cout << "Echec lecture du fichier de grille : " << path << "\n";
        }
        REQUIRE( ok );
        // Always dump expected vs actual for file IO
        Grid expectedFile;
        expectedFile.setGridDimensions(3,3);
        expectedFile.setCell(0,0,true);
        expectedFile.setCell(0,2,true);
        expectedFile.setObstacle(1,1,true);
        expectedFile.setCell(2,0,true);
        std::cout << "File IO - attendu vs obtenu:\n";
        dumpExpectedActual(expectedFile, g);

        if (!(g.getR() == 3 && g.getC() == 3)) {
            std::cout << "Incoherence dimensions fichier :\n";
            std::cout << gridToString(g) << "\n";
        }
    REQUIRE( g.getR() == 3 );
    REQUIRE( g.getC() == 3 );
    if (!g.isObstacle(1,1)) {
        std::cout << "Obstacle manquant a (1,1) :\n";
        std::cout << gridToString(g) << "\n";
    }
    REQUIRE( g.isObstacle(1,1) );
    if (!g.getCell(0,0)) {
        std::cout << "Attendu cellule (0,0) vivante mais elle est morte :\n";
        std::cout << gridToString(g) << "\n";
    }
    REQUIRE( g.getCell(0,0) );

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[FIN] entree_sortie_fichier (" << ms << " ms)\n";
}
