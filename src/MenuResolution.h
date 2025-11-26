//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_MENURESOLUTION_H
#define OOP_MENURESOLUTION_H

#include <SFML/Graphics.hpp>

class MenuResolution {
public:
  static std::vector<sf::VideoMode> getAvailableResolutions();
  static std::string resolutionToString(const sf::VideoMode &mode);
};

#endif // OOP_MENURESOLUTION_H
