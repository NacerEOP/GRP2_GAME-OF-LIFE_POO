# Game of Life (POO) — GRP2

Ce dépôt contient une implémentation orientée objet du jeu de la vie en C++17.

Résumé
- But : simulation du Game of Life (Conway) avec :
	- architecture par composants (Core / Services / UI / Input),
	- grille composée de cellules polymorphes (`Alive` / `Dead` / `Obstacle`),
	- règles encapsulées (`GameRules` / `ConwayRules`),
	- interface console et interface graphique SFML,
	- support pour grille torique (wrapping), cellules obstacles, presets et parallélisation du calcul.

Prérequis
- Compilateur C++17 (g++/clang++/MSVC) ou environnement MinGW sous Windows.
- Pour l'interface graphique : SFML (version compatible avec le code). Assurez-vous que les headers et bibliothèques SFML sont installés et disponibles lors de la compilation.

Compilation rapide (mode console)
- Exemple (commande minimale pour compiler la version console) :

```powershell
g++ -std=c++17 -Isrc src/main.cpp src/Core/Cell.cpp src/Core/Grid.cpp src/Core/GameRules.cpp src/Services/GameService.cpp src/UI/ConsoleUI.cpp src/Input/ConsoleInput.cpp src/Services/FileService.cpp -o TestConsole.exe
```

- Le dépôt contient un script `Compile.bat` qui automatise la compilation, télécharge la dépendance de test (`Catch2`) si nécessaire, construit les binaires et exécute les tests. Sous Windows, lancez :

```powershell
.\Compile.bat
```

Exécution
- Mode console : exécutez le binaire console (par ex. `TestConsole.exe`).
- Mode SFML (graphique) : utilisez la cible graphique construite par `Compile.bat` (ou compilez la cible SFML en ajoutant les sources UI et liant SFML). Veillez à copier les DLL SFML dans le dossier d'exécution si nécessaire.

Presets inclus
La méthode `GameService::loadPreset(int slot)` fournit plusieurs motifs prêts à l'emploi. Slots disponibles :
- `0` : grille vide
- `1` : Glider
- `2` : Blinker (oscillateur)
- `3` : Toad (oscillateur)
# Game of Life — Projet POO (C++17 + SFML)

Ce dépôt contient une implémentation modulaire et testable du Jeu de la Vie (Conway) en C++17. Le projet offre deux modes d'utilisation :
- Mode console (batch / export d'états)
- Mode graphique (interface SFML interactive)

Ce README couvre : installation, compilation, exécution, tests unitaires, architecture logicielle, presets et extensions, et conseils pour l'intégration continue.

## Contenu et objectifs
- Implémentation orientée objet : séparation claire entre la logique métier (Core), les services (Services), la gestion des entrées (Input) et l'interface (UI).
- Types de cellules polymorphes (`Alive`, `Dead`, `Obstacle`).
- Règles encapsulées via `GameRules` (pattern Strategy) — implémentation `ConwayRules` par défaut.
- Support optionnel : grille torique, cellules obstacles, presets, parallélisation des calculs, sons pour l'IU.

## Arborescence principale
- `src/Core/` : classes fondamentales (`Cell`, `Grid`, `GameRules`).
- `src/Services/` : orchestrateurs (`GameService`), I/O (`FileService`), audio (`SoundService`).
- `src/Input/` : gestion des événements (console et SFML) séparée de l'IU.
- `src/UI/` : rendu et interface SFML / console.
- `tests/` : tests unitaires (Catch2 vendored).
- `resources/` : polices, sons, presets.

## Pré-requis
- Compilateur C++ compatible C++17 (g++, clang++ ou MSVC). Sous Windows, MinGW-w64 recommandé pour g++.
- Pour l'interface graphique et audio : SFML compatible (headers et libs disponibles sur le système).

Remarque : la compilation des tests et du binaire console n'exige pas SFML ; la compilation de l'exécutable graphique l'exige.

## Compilation (Windows - PowerShell)
Le projet contient un script `Compile.bat` qui :
- télécharge Catch2 si nécessaire (header single-file),
- compile les tests et l'application (console + SFML si SFML trouvé),
- exécute les tests et copie les DLLs nécessaires si demandé.

Commande :
```powershell
.
Compile.bat
```

Notes pratiques :
- Si `Compile.bat` échoue à lier SFML, vérifiez la variable `SFML_PATH` dans le script ou installez SFML et mettez à jour le chemin.
- Les DLL SFML (graphics/window/system/audio) doivent être accessibles à l'exécutable si vous compilez en mode dynamique.

## Exécution

- Mode console : exécuter le binaire console (par ex. `bin\TestConsole.exe` selon le script). Utilisé pour traitements batch et export d'états.

- Mode graphique (SFML) : exécuter `bin\GameOfLife.exe` (si construit). L'UI propose : Play/Pause, Start/Stop, réglage du tick (ms), toggle Toric, placer/retirer cellules et obstacles, presets, et un top-bar responsive.

Assurez-vous que le dossier `resources/` (fonts, sounds) est accessible depuis le répertoire d'exécution.

## Tests unitaires
- Framework : Catch2 (single-header `tests/catch.hpp`).
- Fichier principal des tests : `tests/test_game.cpp`.

Exécution :
```powershell
.
Compile.bat
.
bin\test_game.exe > bin\test_results.txt
```

Les tests incluent des scénarios pour : blinker, block stable, toric wrapping, obstacles, et lecture/écriture (FileService). Chaque test affiche `EXPECTED` vs `ACTUAL` pour faciliter le diagnostic.

Pour ajouter un test :
1. Ouvrir `tests/test_game.cpp`.
2. Ajouter un `TEST_CASE("Nom du test")` avec initialisation et assertions (`REQUIRE`).
3. Recompiler et exécuter les tests.

## Architecture et design (détaillé)

Principales couches :
- Core (logique métier) :
	- `Grid` : stockage 2D des cellules, helpers pour voisinage (avec ou sans wrapping).
	- `Cell` : hiérarchie polymorphique (`AliveCell`, `DeadCell`, `ObstacleCell`).
	- `GameRules` : interface de stratégie, `ConwayRules` implémente la règle classique.

- Services :
	- `GameService` : orchestre la simulation, `step()` (parallélisé), détection de stabilisation, chargement de presets.
	- `FileService` : lecture/écriture d'états et gestion d'exports.
	- `SoundService` : charge et joue `Click.mp3`, `Hover.mp3`, `SimStartStop.mp3` via SFML Audio (si présent).

- Input & UI :
	- `SFMLInput` : centralise la gestion des événements SFML (hit-tests, mapping de boutons), détachée du rendu.
	- `SFMLUI` : rendering, top-bar responsive, dessin de la grille selon `cellSize` calculé dynamiquement et recentrage automatique.

Décisions clés et justification :
- Polymorphisme pour cellules : facilite extension (nouveaux types de cellules) et respecte OCP.
- Découplage UI / métier : tests sans SFML, réutilisabilité et simplicité d'évolution.
- `unique_ptr` et RAII : gestion sûre de la mémoire.
- Parallélisation simple par plage de lignes : trade-off entre simplicité et performance — évolution possible vers thread-pool.

## Presets et motifs
Plusieurs presets sont fournis via `GameService::loadPreset(slot)` : glider, blinker, toad, block, R-pentomino, Gosper Glider Gun, Pulsar, LWSS, Diehard, et autres motifs utilitaires. Les presets volumineux sont automatiquement centrés.

## Contrôles UI (SFML)
- Play / Pause : démarrer / arrêter la simulation.
- Start / Pause (boutons) : contrôles additionnels pour démarrer ou mettre en pause.
- Déc / Inc : régler le tick (ms) de la simulation.
- Toric toggle : activer/désactiver la grille torique.
- Main Menu : revenir à l'écran d'accueil (l'appel à `updateView()` s'assure que l'UI est recentrée après resize).
- Clic gauche : basculer état d'une cellule.
- Clic droit : basculer obstacle.

## Son (optionnel)
Le projet inclut un `SoundService` qui tente de charger `resources/sounds/Click.mp3`, `Hover.mp3`, `SimStartStop.mp3`. Lorsque présents et si SFML Audio est lié, les sons sont joués sur clics et événements UI.

## Debug et troubleshooting
- Erreur de lien SFML : vérifiez `SFML_PATH` dans `Compile.bat` ou installez SFML et mettez à jour le PATH/DLLs.
- Ressources manquantes : vérifiez que `resources/` est copié à côté de l'exécutable.
- Tests qui échouent : consultez `bin/test_results.txt` pour `EXPECTED` vs `ACTUAL` et isolez le cas de test.



## UML et livrable
- Le dossier `docs/` (recommandé) doit contenir : diagrammes de classes, diagrammes d'activité et de séquence illustrant le cycle d'une itération (UI → GameService → Grid → Cell). Ces diagrammes permettent de valider la conception avant le code.

## Contribution
- Fork → branche feature → PR. Respectez les conventions de code et ajoutez des tests pour toute correction/feature.

## Licence
- Voir `LICENSE` à la racine du dépôt.

---

Si vous voulez, je peux :
- générer un `tests/README.md` expliquant en détail comment écrire un test et donnant un exemple prêt à coller ;
- ajouter un workflow GitHub Actions YAML pour compiler et exécuter les tests automatiquement ;
- produire des diagrammes PlantUML basés sur la structure du code (prêts à exporter en PNG/SVG).

Bonne continuation !
