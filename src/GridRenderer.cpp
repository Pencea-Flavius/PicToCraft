#include "GridRenderer.h"
#include "ShadowedText.h"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>

GridRenderer::GridRenderer(Grid &g, float size, sf::Vector2f off)
    : grid(g), cellSize(size), offset(off), fontLoaded(false), lastMistakes(0) {
    fontLoaded = font.openFromFile("assets/Monocraft.ttf");
    if (!webTexture.loadFromFile("assets/cobweb.png")) {
        std::cerr << "Failed to load cobweb texture" << std::endl;
    }

    // Load break textures
    for (int i = 0; i <= 9; ++i) {
        sf::Texture tex;
        std::string path =
                "assets/break/destroy_stage_" + std::to_string(i) + ".png";
        if (!tex.loadFromFile(path)) {
            std::cerr << "Failed to load " << path << std::endl;
        }
        breakTextures.push_back(tex);
    }
}

void GridRenderer::drawGameInfo(sf::RenderWindow &window) const {
    if (!fontLoaded)
        return;

    auto winSize = window.getSize();

    unsigned int fontSize = static_cast<unsigned int>(winSize.y * 0.035f);
    fontSize = std::max(18u, std::min(fontSize, 40u));

    sf::Text infoText(font, "");
    infoText.setCharacterSize(fontSize);
    infoText.setFillColor(sf::Color::Black);

    if (grid.shouldDisplayScore()) {
        std::ostringstream oss;
        oss << "Scor: " << grid.get_score();
        infoText.setString(oss.str());

        float padding = winSize.x * 0.015f;
        padding = std::max(10.f, std::min(padding, 30.f));

        auto bounds = infoText.getLocalBounds();
        infoText.setPosition(
            {
                winSize.x - bounds.size.x - bounds.position.x - padding,
                padding - bounds.position.y
            });

        float scaleX = static_cast<float>(winSize.x) / 1280.0f;
        float scaleY = static_cast<float>(winSize.y) / 720.0f;
        float scale = std::min(scaleX, scaleY);

        ShadowedText::draw(window, infoText, scale);
    } else {
        int currentMistakes = grid.get_mistakes();
        if (grid.get_mistakes() > lastMistakes) {
            heartDisplay.triggerFlash();
        }
        lastMistakes = grid.get_mistakes();

        float scaleX = static_cast<float>(winSize.x) / 1280.0f;
        float scaleY = static_cast<float>(winSize.y) / 720.0f;
        float baseScale = std::min(scaleX, scaleY);

        float heartScale = baseScale * 2.5f; // Make hearts 2.5x larger

        float padding = winSize.x * 0.015f;
        padding = std::max(10.f, std::min(padding, 30.f));

        int maxMistakes = grid.get_max_mistakes();
        int totalHearts = (maxMistakes + 1) / 2;
        float heartWidth = 9.0f * heartScale;
        float totalWidth = totalHearts * heartWidth;

        sf::Vector2f pos(winSize.x - totalWidth - padding, padding);

        heartDisplay.update(animationClock.restart().asSeconds());
        heartDisplay.draw(window, currentMistakes, maxMistakes, pos, heartScale,
                          grid.is_time_mode());
    }
}

void GridRenderer::draw(sf::RenderWindow &window) const {
    const auto &hints = grid.get_hints();
    int n = grid.get_size();

    size_t maxRowWidth = hints.get_max_row_width();
    size_t maxColHeight = hints.get_max_col_height();

    float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
    float colHintsHeight = maxColHeight * cellSize * 0.8f;

    sf::Vector2f gridOffset = {
        offset.x + rowHintsWidth,
        offset.y + colHintsHeight
    };

    if (fontLoaded) {
        const auto &rowHints = hints.get_row_hints();
        for (size_t i = 0; i < rowHints.size(); ++i) {
            float rowY = gridOffset.y + i * cellSize + cellSize * 0.5f;
            float startX = gridOffset.x - 20.f;

            for (int j = static_cast<int>(rowHints[i].size()) - 1; j >= 0; --j) {
                sf::Vector2f pos(startX - cellSize * 0.25f, rowY);

                if (grid.isHintWebbed(true, i, j)) {
                    sf::Sprite webSprite(webTexture);
                    float scale =
                            (cellSize * 0.6f) / static_cast<float>(webTexture.getSize().x);
                    webSprite.setScale({scale, scale});
                    webSprite.setOrigin(
                        {
                            static_cast<float>(webTexture.getSize().x) / 2.f,
                            static_cast<float>(webTexture.getSize().y) / 2.f
                        });

                    sf::Text tempText(font, std::to_string(rowHints[i][j]));
                    tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
                    auto bounds = tempText.getLocalBounds();

                    float textX = startX - bounds.size.x - bounds.position.x;
                    float textY = rowY - bounds.size.y * 0.5f - bounds.position.y;

                    // Center web on the text area
                    webSprite.setPosition(
                        {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f});
                    window.draw(webSprite);

                    // Draw break texture
                    int health = grid.get_hints().getWebHealth(true, i, j);
                    if (health < 10) {
                        int stage = 9 - health; // 9 down to 0
                        if (stage >= 0 && stage < static_cast<int>(breakTextures.size())) {
                            sf::Sprite breakSprite(breakTextures[stage]);
                            breakSprite.setScale({scale, scale});
                            breakSprite.setOrigin(webSprite.getOrigin());
                            breakSprite.setPosition(webSprite.getPosition());
                            window.draw(breakSprite);
                        }
                    }
                } else {
                    sf::Text text(font, std::to_string(rowHints[i][j]));
                    text.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
                    text.setFillColor(sf::Color::Black);

                    auto bounds = text.getLocalBounds();
                    text.setPosition({
                        startX - bounds.size.x - bounds.position.x,
                        rowY - bounds.size.y * 0.5f - bounds.position.y
                    });

                    window.draw(text);
                }
                startX -= cellSize * 0.8f;
            }
        }

        const auto &colHints = hints.get_col_hints();
        for (size_t j = 0; j < colHints.size(); ++j) {
            float colX = gridOffset.x + j * cellSize + cellSize * 0.5f;
            float startY = gridOffset.y - 10.f;

            for (int i = static_cast<int>(colHints[j].size()) - 1; i >= 0; --i) {
                if (grid.isHintWebbed(false, j, i)) {
                    sf::Sprite webSprite(webTexture);
                    float scale =
                            (cellSize * 0.6f) / static_cast<float>(webTexture.getSize().x);
                    webSprite.setScale({scale, scale});
                    webSprite.setOrigin(
                        {
                            static_cast<float>(webTexture.getSize().x) / 2.f,
                            static_cast<float>(webTexture.getSize().y) / 2.f
                        });

                    sf::Text tempText(font, std::to_string(colHints[j][i]));
                    tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
                    auto bounds = tempText.getLocalBounds();

                    float textX = colX - bounds.size.x * 0.5f - bounds.position.x;
                    float textY = startY - bounds.size.y - bounds.position.y;

                    webSprite.setPosition(
                        {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f});
                    window.draw(webSprite);

                    // Draw break texture
                    int health = grid.get_hints().getWebHealth(false, j, i);
                    if (health < 10) {
                        int stage = 9 - health; // 9 down to 0
                        if (stage >= 0 && stage < static_cast<int>(breakTextures.size())) {
                            sf::Sprite breakSprite(breakTextures[stage]);
                            breakSprite.setScale({scale, scale});
                            breakSprite.setOrigin(webSprite.getOrigin());
                            breakSprite.setPosition(webSprite.getPosition());
                            window.draw(breakSprite);
                        }
                    }
                } else {
                    sf::Text text(font, std::to_string(colHints[j][i]));
                    text.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
                    text.setFillColor(sf::Color::Black);

                    auto bounds = text.getLocalBounds();
                    text.setPosition({
                        colX - bounds.size.x * 0.5f - bounds.position.x,
                        startY - bounds.size.y - bounds.position.y
                    });

                    window.draw(text);
                }
                startY -= cellSize * 0.8f;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            sf::RectangleShape rect(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
            rect.setPosition(
                {gridOffset.x + j * cellSize, gridOffset.y + i * cellSize});

            if (grid.get_block(i, j).is_completed())
                rect.setFillColor(sf::Color::Black);
            else
                rect.setFillColor(sf::Color::White);

            rect.setOutlineColor(sf::Color(180, 180, 180));
            rect.setOutlineThickness(1.f);
            window.draw(rect);
        }
    }

    grid.drawMode(window);
}

void GridRenderer::handleClick(const sf::Vector2i &mousePos) const {
    const auto &hints = grid.get_hints();
    size_t maxRowWidth = hints.get_max_row_width();
    size_t maxColHeight = hints.get_max_col_height();

    float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
    float colHintsHeight = maxColHeight * cellSize * 0.8f;

    sf::Vector2f gridOffset = {
        offset.x + rowHintsWidth,
        offset.y + colHintsHeight
    };

    if (mousePos.x < gridOffset.x || mousePos.y < gridOffset.y)
        return;

    int n = grid.get_size();
    int x = static_cast<int>((mousePos.y - gridOffset.y) / cellSize);
    int y = static_cast<int>((mousePos.x - gridOffset.x) / cellSize);

    if (x >= 0 && x < n && y >= 0 && y < n)
        grid.toggle_block(x, y);
}

sf::Vector2f GridRenderer::getHintCenter(bool isRow, int line,
                                         int index) const {
    const auto &hints = grid.get_hints();
    size_t maxRowWidth = hints.get_max_row_width();
    size_t maxColHeight = hints.get_max_col_height();

    float rowHintsWidth = maxRowWidth * cellSize * 0.8f;
    float colHintsHeight = maxColHeight * cellSize * 0.8f;

    sf::Vector2f gridOffset = {
        offset.x + rowHintsWidth,
        offset.y + colHintsHeight
    };

    if (isRow) {
        const auto &rowHints = hints.get_row_hints();
        if (line >= rowHints.size() || index >= rowHints[line].size())
            return {0, 0};

        float rowY = gridOffset.y + line * cellSize + cellSize * 0.5f;
        float startX = gridOffset.x - 20.f;

        // Calculate X position based on index (reverse order)
        int reverseIndex = rowHints[line].size() - 1 - index;
        float currentX = startX - reverseIndex * (cellSize * 0.8f);

        sf::Text tempText(font, std::to_string(rowHints[line][index]));
        tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
        auto bounds = tempText.getLocalBounds();

        float textX = currentX - bounds.size.x - bounds.position.x;
        float textY = rowY - bounds.size.y * 0.5f - bounds.position.y;

        return {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f};
    } else {
        const auto &colHints = hints.get_col_hints();
        if (line >= colHints.size() || index >= colHints[line].size())
            return {0, 0};

        float colX = gridOffset.x + line * cellSize + cellSize * 0.5f;
        float startY = gridOffset.y - 10.f;

        int reverseIndex = colHints[line].size() - 1 - index;
        float currentY = startY - reverseIndex * (cellSize * 0.8f);

        sf::Text tempText(font, std::to_string(colHints[line][index]));
        tempText.setCharacterSize(static_cast<unsigned int>(cellSize * 0.5f));
        auto bounds = tempText.getLocalBounds();

        float textX = colX - bounds.size.x * 0.5f - bounds.position.x;
        float textY = currentY - bounds.size.y - bounds.position.y;

        return {textX + bounds.size.x / 2.f, textY + bounds.size.y / 2.f};
    }
}

void GridRenderer::handleHintClick(const sf::Vector2i &mousePos) const {
    const auto &hints = grid.get_hints();

    // Check all webbed hints to see if clicked
    const auto &rowHints = hints.get_row_hints();
    for (size_t i = 0; i < rowHints.size(); ++i) {
        for (size_t j = 0; j < rowHints[i].size(); ++j) {
            if (grid.isHintWebbed(true, i, j)) {
                sf::Vector2f center = getHintCenter(true, i, j);
                float radius = cellSize * 0.4f; // Approximate hit radius

                float dx = mousePos.x - center.x;
                float dy = mousePos.y - center.y;
                if (dx * dx + dy * dy <= radius * radius) {
                    const_cast<Grid &>(grid).damageWeb(true, i, j); // Use damageWeb
                    return; // Handle one click at a time
                }
            }
        }
    }

    const auto &colHints = hints.get_col_hints();
    for (size_t j = 0; j < colHints.size(); ++j) {
        for (size_t i = 0; i < colHints[j].size(); ++i) {
            if (grid.isHintWebbed(false, j, i)) {
                sf::Vector2f center = getHintCenter(false, j, i);
                float radius = cellSize * 0.4f;

                float dx = mousePos.x - center.x;
                float dy = mousePos.y - center.y;
                if (dx * dx + dy * dy <= radius * radius) {
                    const_cast<Grid &>(grid).damageWeb(false, j, i); // Use damageWeb
                    return;
                }
            }
        }
    }
}