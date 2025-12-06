#include "SoundService.h"
#include <filesystem>
#include <iostream>

SoundService::SoundService() {
    namespace fs = std::filesystem;
    // For each logical sound, try a small list of candidate paths so each
    // specific file (Click.mp3, Hover.mp3, SimStartStop.mp3) is searched
    // independently and will not be confused with the others.
    fs::path srcPath = fs::path(__FILE__);
    fs::path projectRoot;
    if (srcPath.has_parent_path()) projectRoot = srcPath.parent_path().parent_path().parent_path();

    std::vector<fs::path> clickCandidates = {
        fs::path("resources/sounds/Click.mp3")
    };
    std::vector<fs::path> hoverCandidates = {
        fs::path("resources/sounds/Hover.mp3")
    };
    std::vector<fs::path> startStopCandidates = {
        fs::path("resources/sounds/SimStartStop.mp3")
    };

    if (!projectRoot.empty()) {
        clickCandidates.emplace_back(projectRoot / "resources" / "sounds" / "Click.mp3");
        hoverCandidates.emplace_back(projectRoot / "resources" / "sounds" / "Hover.mp3");
        startStopCandidates.emplace_back(projectRoot / "resources" / "sounds" / "SimStartStop.mp3");
    }

    bool clickLoaded = false;
    bool hoverLoaded = false;
    bool startstopLoaded = false;

    auto tryLoad = [&](const std::vector<fs::path>& list, sf::SoundBuffer &buf)->bool {
        for (const auto &p : list) {
            try {
                std::string s = p.string();
                if (buf.loadFromFile(s)) return true;
            } catch (...) {}
        }
        return false;
    };

    clickLoaded = tryLoad(clickCandidates, bufClick);
    hoverLoaded = tryLoad(hoverCandidates, bufHover);
    startstopLoaded = tryLoad(startStopCandidates, bufStartStop);

    if (!clickLoaded) std::cerr << "Warning: Click.mp3 not found (no click sounds)\n";
    if (!hoverLoaded) std::cerr << "Warning: Hover.mp3 not found (no hover sounds)\n";
    if (!startstopLoaded) std::cerr << "Warning: SimStartStop.mp3 not found (no start/stop sounds)\n";

    if (clickLoaded) {
        sndClick = std::make_unique<sf::Sound>(bufClick);
        hasClick = true;
    }
    if (hoverLoaded) {
        sndHover = std::make_unique<sf::Sound>(bufHover);
        hasHover = true;
    }
    if (startstopLoaded) {
        sndStartStop = std::make_unique<sf::Sound>(bufStartStop);
        hasStartStop = true;
    }

    ok = hasClick || hasHover || hasStartStop;
}

void SoundService::playClick() {
    if (hasClick) {
        sndClick->stop();
        sndClick->play();
    }
}

void SoundService::playHover() {
    if (hasHover) {
        sndHover->stop();
        sndHover->play();
    }
}

void SoundService::playSimStartStop() {
    if (hasStartStop) {
        sndStartStop->stop();
        sndStartStop->play();
    }
}
