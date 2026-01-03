#include "AlchemyMode.h"
#include "GameManager.h"
#include "Grid.h"
#include <random>
#include <algorithm>

AlchemyMode::AlchemyMode(std::unique_ptr<GameMode> mode) 
    : GameModeDecorator(std::move(mode)), spawnTimer(0.f), spawnInterval(2.0f), 
      breakSound(breakBuffer), spawnSound(spawnBuffer) {

    effectDisplay.loadAssets();

    if (spawnBuffer.loadFromFile("assets/sound/Bow_shoot.ogg")) {
        spawnSound.setBuffer(spawnBuffer);
    }
    
    glassDigBuffers.reserve(3);
    for (int i = 1; i <= 3; ++i) {
        sf::SoundBuffer buf;
        if (buf.loadFromFile("assets/sound/Glass_dig" + std::to_string(i) + ".ogg")) {
            glassDigBuffers.push_back(buf);
        }
    }
    
    static std::vector<sf::Texture> particleTextures;
    if (particleTextures.empty()) { 

         for (int i = 0; i <= 7; ++i) {
             sf::Texture tex;
             std::string path = "assets/particle/potion/effect_" + std::to_string(i) + ".png";
             if (!tex.loadFromFile(path)) {
                 sf::Image img; 
                 img.resize({8, 8}, sf::Color::White); 
                 (void)tex.loadFromImage(img);
             }
             particleTextures.push_back(std::move(tex));
         }
    }
    
    potionSplashSystem.clearTextures();
    for(const auto& t : particleTextures) {
        potionSplashSystem.addTexture(&t);
    }
}

void AlchemyMode::update(float deltaTime) {
    GameModeDecorator::update(deltaTime);
    

    potionSplashSystem.update(deltaTime);
    witchMagicSystem.update(deltaTime);
    
    updateEffects(deltaTime);
    
    if (cachedWindow) {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(*cachedWindow);
        
        if (!mouseInitialized) {
            lastMousePos = currentMousePos;
            mouseInitialized = true;
        }
        
        sf::Vector2i delta = currentMousePos - lastMousePos;
        
        float speedMult = getCursorSpeedMultiplier();
        if (speedMult != 1.0f) {
            delta.x = static_cast<int>(static_cast<float>(delta.x) * speedMult);
            delta.y = static_cast<int>(static_cast<float>(delta.y) * speedMult);
            currentMousePos = lastMousePos + delta;
        }
        
        sf::Vector2f levitationOffset = getLevitationOffset(deltaTime);
        if (levitationOffset.y != 0.0f) {
            currentMousePos.y += static_cast<int>(levitationOffset.y);
        }
        
        if (speedMult != 1.0f || levitationOffset.y != 0.0f) {
            sf::Mouse::setPosition(currentMousePos, *cachedWindow);
        }
        
        lastMousePos = currentMousePos;
    }
    
    if (isLost()) {
        potions.clear();
        activeEffects.clear();
        return;
    }

    effectTickTimer += deltaTime;
    if (effectTickTimer >= EFFECT_TICK_RATE) {
        effectTickTimer = 0.0f;
        
        if (hasEffect(EffectType::Poison)) {
            if (grid && grid->get_mistakes() < grid->get_max_mistakes() - 1) {
                const_cast<Grid*>(grid)->damagePlayer();
            }
        }
        
        if (hasEffect(EffectType::Regeneration)) {
            if (wrappedMode && wrappedMode->getMistakes() > 0) { 
                 wrappedMode->setMistakes(wrappedMode->getMistakes() - 1);
            }
        }
    }

    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.f;
        potions.push_back(PotionFactory::getInstance().createRandomPotion(windowSize)); 
        spawnSound.play(); 
    }
    
    // Removed targetPos unused variable
    
    for (auto it = potions.begin(); it != potions.end();) {
        (*it)->update(deltaTime, mousePos, windowSize);
        
        bool removed = false;
        
        bool hitboxCollision = false;
        sf::Vector2f collisionPoint = mousePos;
        

        
        for (int i = 0; i < 13; ++i) {
             float angle = static_cast<float>(i) * 0.5f;
             if (angle > 6.28f) break;

            sf::Vector2f checkPoint = mousePos + sf::Vector2f(std::cos(angle) * hitboxRadius * 0.5f, 
                                                               std::sin(angle) * hitboxRadius * 0.5f);
            if ((*it)->checkCollision(checkPoint)) {
                hitboxCollision = true;
                collisionPoint = checkPoint;
                break;
            }
        }

        if (!hitboxCollision && (*it)->checkCollision(mousePos)) {
            hitboxCollision = true;
            collisionPoint = mousePos;
        }
        
        if (hitboxCollision) {
            float normalizedHeight = static_cast<float>(windowSize.y) / 1440.f;
            float scale = std::pow(normalizedHeight, 1.2f);
            if(scale < 0.6f) scale = 0.6f;
            
            potionSplashSystem.emit(collisionPoint, 20, (*it)->getColor(), scale);

            


            if (!glassDigBuffers.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, static_cast<int>(glassDigBuffers.size()) - 1);
                
                int idx = dis(gen);
                breakSound.setBuffer(glassDigBuffers[idx]);
                breakSound.play();
            }
            
            if (!(*it)->isBad()) {
                if (grid) {
                    grid->healWebs();
                }
            }

            static std::random_device rd;
            static std::mt19937 gen(rd());
            
            std::uniform_real_distribution<float> durationDist(5.0f, 30.0f);
            float duration = durationDist(gen);
            
            EffectType effectType = colorToEffect((*it)->getColor());
            
            if (effectType == EffectType::InstantHealth) {
                if (wrappedMode) {
                    int currentMistakes = wrappedMode->getMistakes();
                    int newMistakes = std::max(0, currentMistakes - 8);
                    wrappedMode->setMistakes(newMistakes);
                }
            } else if (effectType == EffectType::InstantDamage) {
                if (grid) {
                    for (int i = 0; i < 8 && grid->get_mistakes() < grid->get_max_mistakes(); ++i) {
                        const_cast<Grid*>(grid)->damagePlayer(i == 0); // Play sound only on first hit
                    }
                }
            } else {
                addEffect(effectType, duration);
            }
            
            it = potions.erase(it);
            removed = true;
        }
        
        if (!removed && (*it)->isDead()) {
            it = potions.erase(it);
            removed = true;
        }
        
        if (!removed) {
            ++it;
        }
    }
}

void AlchemyMode::draw(sf::RenderWindow& window) const {
    cachedWindow = &window;
    
    windowSize = window.getSize();
    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window)) + hitboxOffset;
    
    GameModeDecorator::draw(window);
    
    for (const auto& p : potions) {
        p->draw(window);
    }
    
    const_cast<AlchemyMode*>(this)->potionSplashSystem.draw(window);
    const_cast<AlchemyMode*>(this)->witchMagicSystem.draw(window);
    

    
    if (hasEffect(EffectType::Blindness)) {
        static sf::RenderTexture blindLayer;
        static sf::Texture blindLightTexture;
        static std::optional<sf::Sprite> blindLightSprite;
        static bool blindLightCreated = false;
        
        if (!blindLightCreated) {
            unsigned int size = 500;
            sf::Image image;
            image.resize({size, size}, sf::Color::Transparent);
            
            float centerX = static_cast<float>(size) / 2.0f;
            float centerY = static_cast<float>(size) / 2.0f;
            float radius = static_cast<float>(size) / 2.0f;
            
            for (unsigned int y = 0; y < size; ++y) {
                for (unsigned int x = 0; x < size; ++x) {
                    float dx = static_cast<float>(x) - centerX;
                    float dy = static_cast<float>(y) - centerY;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    
                    if (distance <= radius) {
                        float alpha = 255.0f * (1.0f - distance / radius);
                        image.setPixel({x, y}, sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
                    }
                }
            }
            
            (void)blindLightTexture.loadFromImage(image);
            blindLightTexture.setSmooth(true);
            blindLightCreated = true;
        }
        
        sf::Vector2u currentSize = window.getSize();
        if (blindLayer.getSize() != currentSize) {
            (void)blindLayer.resize(currentSize);
        }
        
        blindLayer.clear(sf::Color(0, 0, 0, 255));
        
        if (!blindLightSprite) {
            blindLightSprite.emplace(blindLightTexture);
        }
        blindLightSprite->setOrigin({static_cast<float>(blindLightTexture.getSize().x) / 2.0f,
                                     static_cast<float>(blindLightTexture.getSize().y) / 2.0f});
        
        sf::Vector2i screenMousePos = sf::Mouse::getPosition(window);
        blindLightSprite->setPosition(static_cast<sf::Vector2f>(screenMousePos));
        
        float scale = static_cast<float>(window.getSize().x) / 1920.0f;
        blindLightSprite->setScale({scale, scale});
        

        
        sf::BlendMode subtractAlpha(
            sf::BlendMode::Factor::Zero, sf::BlendMode::Factor::One,
            sf::BlendMode::Equation::Add, sf::BlendMode::Factor::One,
            sf::BlendMode::Factor::One, sf::BlendMode::Equation::ReverseSubtract);
        
        blindLightSprite->setColor(sf::Color::White); // Pure white light, no tint
        blindLayer.draw(*blindLightSprite, subtractAlpha);
        
        blindLayer.display();
        
        sf::Sprite overlay(blindLayer.getTexture());
        sf::View originalView = window.getView();
        window.setView(window.getDefaultView());
        window.draw(overlay);
        window.setView(originalView);
    }
    
    // Effects are drawn by GridRenderer next to grid
}

bool AlchemyMode::handleInput(const sf::Event& event, const sf::RenderWindow& window) {
    if (GameModeDecorator::handleInput(event, window)) return true;
    

    if (const auto* mousePress = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePress->button == sf::Mouse::Button::Left) {
            sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePress->position);
            
            for (auto it = potions.begin(); it != potions.end(); ) {
                if ((*it)->checkCollision(worldMousePos)) {
                    float normalizedHeight = static_cast<float>(windowSize.y) / 1440.f;
                    float scale = std::pow(normalizedHeight, 1.2f);
                    if(scale < 0.6f) scale = 0.6f;
                    
                    potionSplashSystem.emit(worldMousePos, 20, (*it)->getColor(), scale);
                    breakSound.play();
                    
                    if (grid != nullptr) {
                        grid->healWebs();
                    }
                    
                    potions.erase(it);
                    return true;
                } else {
                    ++it;
                }
            }
        }
    }
    
    return false;
}

void AlchemyMode::addEffect(EffectType type, float duration) {
    auto it = std::ranges::find_if(activeEffects,
        [type](const ActiveEffect& e) { return e.type == type; });
    
    if (it != activeEffects.end()) {
        if (duration > it->duration) {
            it->duration = duration;
            it->amplifier = 0; 
        }
    } else {
        ActiveEffect effect{};
        effect.type = type;
        effect.duration = duration;
        effect.amplifier = 0;
        activeEffects.push_back(effect);
    }
}

void AlchemyMode::updateEffects(float deltaTime) {
    for (auto& effect : activeEffects) {
        effect.duration -= deltaTime;
    }
    
    std::erase_if(activeEffects, [](const ActiveEffect& e) { return e.duration <= 0.0f; });
}

EffectType AlchemyMode::colorToEffect(const sf::Color& color) {
    std::uint32_t colorHex = (color.r << 16) | (color.g << 8) | color.b;
    
    auto matches = [](std::uint32_t c1, std::uint32_t c2) {
        int r1 = static_cast<int>((c1 >> 16) & 0xFF);
        int g1 = static_cast<int>((c1 >> 8) & 0xFF);
        int b1 = static_cast<int>(c1 & 0xFF);
        
        int r2 = static_cast<int>((c2 >> 16) & 0xFF);
        int g2 = static_cast<int>((c2 >> 8) & 0xFF);
        int b2 = static_cast<int>(c2 & 0xFF);
        
        int diff = std::abs(r1 - r2) + std::abs(g1 - g2) + std::abs(b1 - b2);
        return diff < 50; 
    };
    
    if (matches(colorHex, 0x33EBFF)) return EffectType::Speed;
    if (matches(colorHex, 0xD9C043)) return EffectType::Haste;
    if (matches(colorHex, 0xF82423)) return EffectType::InstantHealth; 
    if (matches(colorHex, 0xFF69B4)) return EffectType::Saturation; 
    if (matches(colorHex, 0xCD5CAB)) return EffectType::Regeneration;
    if (matches(colorHex, 0xC2FF66)) return EffectType::NightVision;
    
    if (matches(colorHex, 0x8BAFE0)) return EffectType::Slowness;
    if (matches(colorHex, 0x4A4217)) return EffectType::MiningFatigue;
    if (matches(colorHex, 0xA9656A)) return EffectType::InstantDamage;
    if (matches(colorHex, 0x1F1F23)) return EffectType::Blindness;
    if (matches(colorHex, 0x587653)) return EffectType::Hunger;
    if (matches(colorHex, 0x484D48)) return EffectType::Weakness;
    if (matches(colorHex, 0x87A363)) return EffectType::Poison;
    if (matches(colorHex, 0xCEFFFF)) return EffectType::Levitation;
    
    return EffectType::Weakness;
}

bool AlchemyMode::hasEffect(EffectType type) const {
    return std::ranges::any_of(activeEffects,
        [type](const ActiveEffect& e) { return e.type == type; });
}

float AlchemyMode::getCursorSpeedMultiplier() const {
    float multiplier = 1.0f;
    
    if (hasEffect(EffectType::Speed)) {
        multiplier *= 2.0f; // Cursor twice as fast
    }
    if (hasEffect(EffectType::Slowness)) {
        multiplier *= 0.5f; // Half speed
    }
    
    return multiplier;
}

sf::Vector2f AlchemyMode::getLevitationOffset(float deltaTime) const {
    if (hasEffect(EffectType::Levitation)) {
        return {0.0f, -200.0f * deltaTime};
    }
    return {0.0f, 0.0f};
}
