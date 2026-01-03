#include "XPBar.h"
#include "Exceptions.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

XPBar::XPBar() : backgroundSprite(backgroundTexture), progressSprite(progressTexture) {
    if (!backgroundTexture.loadFromFile("assets/xp/experience_bar_background.png")) {
        throw AssetLoadException("assets/xp/experience_bar_background.png", "Texture");
    } else {
        backgroundSprite.setTexture(backgroundTexture);
    }
    if (!progressTexture.loadFromFile("assets/xp/experience_bar_progress.png")) {
        throw AssetLoadException("assets/xp/experience_bar_progress.png", "Texture");
    } else {
        progressSprite.setTexture(progressTexture);
    }
    
    if (!font.openFromFile("assets/Monocraft.ttf")) {
        throw AssetLoadException("assets/Monocraft.ttf", "Font");
    }
}

void XPBar::drawOutlinedText(sf::RenderWindow& window, const std::string& text,
                              const sf::Vector2f& position, unsigned int fontSize,
                              sf::Color fillColor, sf::Color outlineColor, float outlineThickness) const {
    const_cast<sf::Texture&>(font.getTexture(fontSize)).setSmooth(false);

    sf::Text textObj(font, text, fontSize);
    
    auto bounds = textObj.getLocalBounds();
    textObj.setOrigin({std::floor(bounds.position.x + bounds.size.x / 2.f),
                       std::floor(bounds.position.y + bounds.size.y / 2.f)});
    
    textObj.setOrigin({std::floor(bounds.position.x + bounds.size.x / 2.f),
                       std::floor(bounds.position.y + bounds.size.y / 2.f)});
    
    textObj.setFillColor(fillColor);
    textObj.setOutlineColor(outlineColor);
    textObj.setOutlineThickness(std::max(1.f, std::ceil(outlineThickness)));

    float baseX = std::floor(position.x);
    float baseY = std::floor(position.y);
    
    textObj.setPosition({baseX, baseY});
    window.draw(textObj);
}

void XPBar::draw(sf::RenderWindow& window, int score, const sf::Vector2f& position, float scale) {
    auto bgSize = backgroundTexture.getSize();
    auto progSize = progressTexture.getSize();
    
    if (bgSize.x == 0 || bgSize.y == 0) return;
    
    int displayScore = score / 100;
    
    float progress = static_cast<float>(displayScore % 10) / 10.f;
    
    if (bgSize.x > 0 && bgSize.y > 0) {
        backgroundSprite.setColor(sf::Color::White);
        backgroundSprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(static_cast<int>(bgSize.x), static_cast<int>(bgSize.y))));
        
        backgroundSprite.setScale({scale, scale});
        backgroundSprite.setOrigin({static_cast<float>(bgSize.x) / 2.f, static_cast<float>(bgSize.y) / 2.f});
        backgroundSprite.setPosition(position);
        window.draw(backgroundSprite);
    }
    

    if (progSize.x > 0 && progSize.y > 0) {
        int fillWidth = std::max(0, static_cast<int>(static_cast<float>(progSize.x) * progress));

        progressSprite.setTextureRect(sf::IntRect({0, 0}, {fillWidth, static_cast<int>(progSize.y)}));
        progressSprite.setScale({scale, scale});
        
        float bgHalfWidth = (static_cast<float>(bgSize.x) / 2.f) * scale;
        float leftEdge = position.x - bgHalfWidth;
        
        progressSprite.setOrigin({0.f, static_cast<float>(progSize.y) / 2.f});
        
        progressSprite.setPosition({leftEdge, position.y});
        window.draw(progressSprite);
    }
    
    if (true) {
        std::ostringstream oss;
        oss << displayScore;
        
        auto fontSize = static_cast<unsigned int>(8.f * scale);
        
        float textY = position.y - (static_cast<float>(bgSize.y) / 2.f * scale) - (2.0f * scale);
        
        sf::Vector2f textPos = {position.x, textY};
        
        drawOutlinedText(window, oss.str(), textPos, fontSize, 
                         sf::Color(127, 255, 0), sf::Color::Black, 1.f * scale);
    }
}
