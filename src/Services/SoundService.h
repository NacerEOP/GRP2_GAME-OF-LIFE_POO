// Simple sound service using SFML audio
#pragma once

#include <string>
#include <SFML/Audio.hpp>
#include <memory>

class SoundService {
public:
    SoundService();
    ~SoundService() = default;

    // Play sounds for UI events
    void playClick();
    void playHover();
    void playSimStartStop();

    bool isOk() const { return ok; }

private:
    bool ok = false;
    sf::SoundBuffer bufClick;
    sf::SoundBuffer bufHover;
    sf::SoundBuffer bufStartStop;
    std::unique_ptr<sf::Sound> sndClick;
    std::unique_ptr<sf::Sound> sndHover;
    std::unique_ptr<sf::Sound> sndStartStop;
    // Flags to indicate which sounds were successfully loaded
    bool hasClick = false;
    bool hasHover = false;
    bool hasStartStop = false;
};
