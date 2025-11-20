#ifndef OOP_MENUPANORAMA_H
#define OOP_MENUPANORAMA_H

#include <SFML/Graphics.hpp>
#include <optional>

class MenuPanorama {
    sf::Texture panoramaTexture;
    std::optional<sf::Sprite> panoramaSprite1;
    std::optional<sf::Sprite> panoramaSprite2;
    bool loaded;
    float offset;
    float speed;
public:
    MenuPanorama();
    ~MenuPanorama() = default;

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

};

#endif //OOP_MENUPANORAMA_H
