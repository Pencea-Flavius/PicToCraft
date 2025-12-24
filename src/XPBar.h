#ifndef XP_BAR_H
#define XP_BAR_H

#include <SFML/Graphics.hpp>

class XPBar {
    sf::Texture backgroundTexture;
    sf::Texture progressTexture;
    sf::Sprite backgroundSprite;
    sf::Sprite progressSprite;
    sf::Font font;
    bool fontLoaded;

public:
    XPBar();

    void draw(sf::RenderWindow& window, int score, const sf::Vector2f& position, float scale);
    
private:

    void drawOutlinedText(sf::RenderWindow& window, const std::string& text,
                          const sf::Vector2f& position, unsigned int fontSize,
                          sf::Color fillColor, sf::Color outlineColor, float outlineThickness) const;
};

#endif // XP_BAR_H
