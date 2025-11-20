//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_MENURESOLUTION_H
#define OOP_MENURESOLUTION_H

#include <SFML/Graphics.hpp>

class MenuResolution {
    MenuResolution() = default;
    ~MenuResolution() = default;
    bool fullscreenChosen_ = false;
public:
    static MenuResolution &getInstance();

    sf::VideoMode selectResolution();

    [[nodiscard]] bool wasFullscreenChosen() const { return fullscreenChosen_; }
    MenuResolution(const MenuResolution &) = delete;
    MenuResolution &operator=(const MenuResolution &) = delete;
    MenuResolution(MenuResolution &&) = delete;
    MenuResolution &operator=(MenuResolution &&) = delete;

};


#endif //OOP_MENURESOLUTION_H
