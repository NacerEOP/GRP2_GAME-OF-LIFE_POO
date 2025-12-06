@echo off
echo ===========================================
echo   COMPILATION GAME OF LIFE
echo ===========================================

REM Créer le dossier bin s'il n'existe pas
if not exist "bin" (
    echo Creation du dossier bin...
    mkdir bin
)

echo.

REM ===========================================
REM Combined build (Console + SFML in one executable)
REM ===========================================
echo [1/1] Compilation combined executable (Console + SFML)...

set "SFML_PATH=C:\Users\NITRO\Documents\SFML Download\SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit\SFML-3.0.2"

echo Utilisation des bibliotheques SANS suffixe -3:
echo   -libsfml-graphics.a
echo   -libsfml-window.a  
echo   -libsfml-system.a
echo.
echo Copie des DLLs AVEC suffixe -3:
echo   -sfml-graphics-3.dll
echo   -sfml-window-3.dll
echo   -sfml-system-3.dll
echo.

g++ -std=c++17 -DUSE_SFML -Isrc -I"%SFML_PATH%\include" ^
src/main.cpp ^
src/Core/Cell.cpp ^
src/Core/Grid.cpp ^
src/Core/GameRules.cpp ^
src/Services/GameService.cpp ^
src/Services/SoundService.cpp ^
src/UI/ConsoleUI.cpp ^
src/Input/ConsoleInput.cpp ^
src/Services/FileService.cpp ^
src/UI/SFMLUI.cpp ^
src/Input/SFMLInput.cpp ^
-o bin/GameOfLife.exe ^
-L"%SFML_PATH%\lib" ^
-lsfml-graphics ^
-lsfml-window ^
-lsfml-system ^
-lsfml-audio ^
-lfreetype ^
-logg ^
-lvorbis ^
-lvorbisenc ^
-lvorbisfile

if %errorlevel% equ 0 (
    echo [OK] SFML version: bin/GameOfLife.exe
    
    echo.
    echo Copie des DLLs SFML...
    copy "%SFML_PATH%\bin\sfml-graphics-3.dll" "bin\" >nul
    copy "%SFML_PATH%\bin\sfml-window-3.dll" "bin\" >nul
    copy "%SFML_PATH%\bin\sfml-system-3.dll" "bin\" >nul
    copy "%SFML_PATH%\bin\sfml-audio-3.dll" "bin\" >nul 2>nul
    copy "%SFML_PATH%\bin\sfml-network-3.dll" "bin\" >nul 2>nul
    
    echo DLLs copiees:
    dir bin\sfml-*.dll /b
    
    REM Copier aussi les DLLs des dependances
    echo.
    echo Recherche des DLLs de dependances...
    if exist "%SFML_PATH%\bin\libfreetype-*.dll" (
        copy "%SFML_PATH%\bin\libfreetype-*.dll" "bin\" >nul 2>nul
    )
    if exist "%SFML_PATH%\bin\libogg*.dll" (
        copy "%SFML_PATH%\bin\libogg*.dll" "bin\" >nul 2>nul
    )
    if exist "%SFML_PATH%\bin\libvorbis*.dll" (
        copy "%SFML_PATH%\bin\libvorbis*.dll" "bin\" >nul 2>nul
    )
    
) else (
    echo [ERREUR] Compilation SFML echouee
    
    echo.
    echo Essai avec bibliotheques statiques...
    g++ -std=c++17 -DUSE_SFML -Isrc -I"%SFML_PATH%\include" ^
    src/main.cpp ^
    src/Core/Cell.cpp ^
    src/Core/Grid.cpp ^
    src/Core/GameRules.cpp ^
    src/Services/GameService.cpp ^
    src/Services/SoundService.cpp ^
    src/UI/ConsoleUI.cpp ^
    src/Input/ConsoleInput.cpp ^
    src/Services/FileService.cpp ^
    src/UI/SFMLUI.cpp ^
    src/Input/SFMLInput.cpp ^
    -o bin/GameOfLife.exe ^
    "%SFML_PATH%\lib\libsfml-graphics.a" ^
    "%SFML_PATH%\lib\libsfml-window.a" ^
    "%SFML_PATH%\lib\libsfml-system.a" ^
    "%SFML_PATH%\lib\libsfml-audio.a" ^
    "%SFML_PATH%\lib\libsfml-main.a" ^
    "%SFML_PATH%\lib\libfreetype.a" ^
    "%SFML_PATH%\lib\libogg.a" ^
    "%SFML_PATH%\lib\libvorbis.a" ^
    "%SFML_PATH%\lib\libvorbisenc.a" ^
    "%SFML_PATH%\lib\libvorbisfile.a"
    
    if %errorlevel% equ 0 (
        echo [OK] Compilation avec bibliotheques statiques reussie!
    )
)

echo.
echo Building unit tests...
REM Ensure Catch2 header is available for tests
echo Ensuring Catch2 header `tests\catch.hpp` is present...
powershell -Command "if(-not (Test-Path 'tests\\catch.hpp')) { Write-Host 'Downloading catch.hpp...'; Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/catchorg/Catch2/v2.13.10/single_include/catch2/catch.hpp' -OutFile 'tests\\catch.hpp' -UseBasicParsing } else { Write-Host 'catch.hpp already present' }"

g++ -std=c++17 -Isrc -Itests tests/test_game.cpp src/Core/Cell.cpp src/Core/Grid.cpp src/Core/GameRules.cpp src/Services/GameService.cpp src/Services/FileService.cpp -o bin/test_game.exe
if %errorlevel% equ 0 (
    echo [OK] Unit tests built: bin/test_game.exe
) else (
    echo [ERREUR] Unit test build failed
)

REM Run tests and capture output to a log file
echo.
echo Running unit tests and saving output to bin\test_results.txt ...
cmd /c "bin\test_game.exe > bin\test_results.txt 2>&1"
if exist bin\test_results.txt (
    echo Test output saved to bin\test_results.txt
) else (
    echo Failed to run tests or produce log file.
)

echo.
echo ===========================================
echo   COMPILATION TERMINEE
echo ===========================================
echo.
echo Executables crees dans bin/:
dir /b bin\*.exe
echo.
echo Pour executer la version SFML, assurez-vous que:
echo 1. Les DLLs sfml-*.dll sont dans bin/
echo 2. Eventuellement copiez aussi libfreetype-6.dll, etc.
echo.
pause