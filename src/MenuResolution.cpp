//
// Created by zzfla on 11/8/2025.
//

#include "MenuResolution.h"
#include <algorithm>
#include <string>

std::vector<sf::VideoMode> MenuResolution::getAvailableResolutions() {
  std::vector<sf::VideoMode> modes = {
      sf::VideoMode({1280, 720}), sf::VideoMode({1600, 900}),
      sf::VideoMode({1920, 1080}), sf::VideoMode({2560, 1440}),
      sf::VideoMode({3840, 2160})};

  auto desktop = sf::VideoMode::getDesktopMode();

  // Check if desktop resolution already exists
  auto it = std::ranges::find_if(
      modes.begin(), modes.end(),
      [&desktop](const sf::VideoMode &m) { return m.size == desktop.size; });

  if (it == modes.end()) {
    modes.push_back(desktop);
  }

  std::ranges::sort(modes.begin(), modes.end(),
            [](const sf::VideoMode &a, const sf::VideoMode &b) {
              return (a.size.x != b.size.x) ? (a.size.x < b.size.x)
                                            : (a.size.y < b.size.y);
            });

  return modes;
}

std::string MenuResolution::resolutionToString(const sf::VideoMode &mode) {
  return std::to_string(mode.size.x) + " x " + std::to_string(mode.size.y);
}