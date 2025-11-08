//
// Created by zzfla on 11/8/2025.
//

#ifndef OOP_MENURESOLUTION_H
#define OOP_MENURESOLUTION_H

#include <SFML/Graphics.hpp>

class MenuResolution {
public:
    static MenuResolution &getInstance();

    sf::VideoMode selectResolution();

    bool wasFullscreenChosen() const { return fullscreenChosen_; }

private:
    MenuResolution() = default;
    ~MenuResolution() = default;

    MenuResolution(const MenuResolution &) = delete;
    MenuResolution &operator=(const MenuResolution &) = delete;
    MenuResolution(MenuResolution &&) = delete;
    MenuResolution &operator=(MenuResolution &&) = delete;

    bool fullscreenChosen_ = false;
};


#endif //OOP_MENURESOLUTION_H
