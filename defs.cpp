#include "defs.h"
#include <iostream>
#include <fstream>
#include <sstream>


void readConfig(const std::string& path) {
    std::ifstream configFile(path);
    if (!configFile) {
        std::cerr << "Unable to open config file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            if (key == "difficulty") {
                int diff = std::stoi(value);
                switch (diff) {
                    case 1: currentDifficulty = Difficulty::Easy; break;
                    case 2: currentDifficulty = Difficulty::Medium; break;
                    case 3: currentDifficulty = Difficulty::Hard; break;
                    default: currentDifficulty = Difficulty::Easy; break;
                }
            } else if (key == "color") {
                int colorValue = std::stoi(value);
                switch (colorValue) {
                    case 0: gameColor = Color::Blue; break;
                    case 1: gameColor = Color::Pink; break;
                    default: gameColor = Color::Blue; break;
                }
            } else if (key == "volume") {
                volume = std::stoi(value);
            } else if (key == "highscore") {
                highscore = std::stoi(value);
            }
        }
    }
    configFile.close();
}


void saveConfig(const std::string& path) {
    std::ofstream configFile(path);
    if (!configFile) {
        std::cerr << "Unable to open config file for writing!" << std::endl;
        return;
    }

    int diffValue = 1;
    switch (currentDifficulty) {
        case Difficulty::Easy: diffValue = 1; break;
        case Difficulty::Medium: diffValue = 2; break;
        case Difficulty::Hard: diffValue = 3; break;
    }

    configFile << "difficulty=" << diffValue << std::endl;
    configFile << "color=" << static_cast<int>(gameColor) << std::endl;
    configFile << "volume=" << volume << std::endl;
    configFile << "highscore=" << highscore << std::endl;

    configFile.close();
}
