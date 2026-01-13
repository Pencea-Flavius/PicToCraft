#include "EndermanMode.h"
#include "Exceptions.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Hitbox definitions
const std::array<EndermanMode::Hitbox, 9> EndermanMode::ENDERMAN_HITBOXES = {
  {
    {286, 19, 111, 172, 0.0f}, // Head
    {260, 175, 117, 124, 20.0f}, // Body
    {206, 142, 32, 152, 75.0f}, // Left Arm Upper
    {102, 227, 36, 197, 25.0f}, // Left Arm Lower
    {395.833f, 192.5f, 25.0f, 103.0f, -25.0f}, // Right Arm Upper
    {424.667f, 279.333f, 26.0f, 167.0f, -10.0f}, // Right Arm Lower
    {188, 271, 26, 344, 25.0f}, // Left Leg
    {374, 278, 31, 172, -35.0f}, // Right Leg Upper
    {435.167f, 424, 36, 182, -10.0f} // Right Leg Lower
  }
};

EndermanMode::EndermanMode(std::unique_ptr<GameMode> mode)
  : GameModeDecorator(std::move(mode)), spawnTimer(0.0f), spawnInterval(3.0f),
    endermanLifetime(0.0f), maxLifetime(30.0f), currentFrame(0),
    animationTimer(0.0f), frameTime(0.15f), endermanVisible(false),
    jumpscareActive(false), playerLost(false), needsRescaling(false),
    hoverTimer(0.0f), endermanOpacity(0.0f), jumpscareTimer(0.0f),
    jumpscareFrame(0), jumpscareFrameTime(0.05f) {
  if (!endermanTexture.loadFromFile("assets/enemy/enderman.png")) {
    throw AssetLoadException("assets/enemy/enderman.png", "Texture");
  }

  if (!jumpscareTexture.loadFromFile("assets/enemy/jumpscare.png")) {
    throw AssetLoadException("assets/enemy/jumpscare.png", "Texture");
  }

  if (jumpscareSoundBuffer.loadFromFile("assets/sound/Enderman_death.ogg")) {
    // Asset loaded
  }

  if (stareSoundBuffer.loadFromFile("assets/sound/Enderman_stare.ogg")) {
    stareSound.emplace(stareSoundBuffer);
  }

  for (int i = 1; i <= 5; ++i) {
    sf::SoundBuffer buffer;
    std::string path =
        "assets/sound/Enderman_idle" + std::to_string(i) + ".ogg";
    if (buffer.loadFromFile(path)) {
      idleBuffers.push_back(buffer);
    }
  }

  for (int i = 1; i <= 4; ++i) {
    sf::SoundBuffer buffer;
    std::string path =
        "assets/sound/Enderman_hurt" + std::to_string(i) + ".ogg";
    if (buffer.loadFromFile(path)) {
      hurtBuffers.push_back(buffer);
    }
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(10.0f, 20.0f);
  spawnInterval = static_cast<float>(dis(gen));

  // Load portal textures
  portalTextures.reserve(8);
  for (int i = 0; i <= 7; ++i) {
      sf::Texture tex;
      if (tex.loadFromFile("assets/particle/generic_" + std::to_string(i) + ".png")) {
          portalTextures.push_back(std::move(tex));
      }
  }
  for(const auto& t : portalTextures) {
      portalSystem.addTexture(&t);
  }

  // Load teleport sounds
  if (!teleport1Buffer.loadFromFile("assets/sound/teleport1.ogg")) {
     throw AssetLoadException("assets/sound/teleport1.ogg", "Sound");
  }
  if (!teleport2Buffer.loadFromFile("assets/sound/teleport2.ogg")) {
     throw AssetLoadException("assets/sound/teleport2.ogg", "Sound");
  }
}

void EndermanMode::update(float deltaTime) {
  GameModeDecorator::update(deltaTime);
  
  portalSystem.update(deltaTime); // Update particles regardless of visibility (trailing effect)

  if (jumpscareActive) {
    updateJumpscareAnimation(deltaTime);
    return;
  }

  if (endermanVisible) {
    endermanLifetime += deltaTime;
    updateEndermanAnimation(deltaTime);
    
    // Update hover logic
    if (cachedWindow) {
      bool hovering = isMouseOverEnderman(*cachedWindow);
      updateStareSound(hovering, deltaTime);
      
      if (hovering) {
        // Mouse over: increase opacity
        hoverTimer += deltaTime * 0.25f; // Scale by deltaTime for smooth animation
        endermanOpacity = std::min(1.0f, 0.4f + hoverTimer * 0.4f);
        
        // Trigger jumpscare when solid
        if (endermanOpacity >= 0.98f) {
          triggerJumpscare();
        }
      } else {
        // Mouse off: decrease opacity
        hoverTimer = std::max(0.0f, hoverTimer - deltaTime * 0.125f);
        endermanOpacity = std::max(0.4f, endermanOpacity - deltaTime * 0.125f);
      }
    }
    
    // Emit particles
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> pDis(0, 2); 
    if (pDis(gen) == 0) { 
        if (endermanSprite) {
             sf::Vector2f spawnPos = endermanSprite->getPosition();

             if (!ENDERMAN_HITBOXES.empty()) {
                 std::uniform_int_distribution<> hbDis(0, static_cast<int>(ENDERMAN_HITBOXES.size()) - 1);
                 const auto& box = ENDERMAN_HITBOXES[hbDis(gen)];
                 
                 sf::Vector2f localCenter(box.x + box.w / 2.0f, box.y + box.h / 2.0f);
                 spawnPos = endermanSprite->getTransform().transformPoint(localCenter);
             }
             
            float pScale = 1.0f;
            if (cachedWindow) {
                float sx = static_cast<float>(cachedWindow->getSize().x) / 1920.0f;
                float sy = static_cast<float>(cachedWindow->getSize().y) / 1080.0f;
                pScale = std::min(sx, sy) * 0.75f;
            }

            portalSystem.emit(spawnPos, 1, sf::Color::White, pScale);
        }
    }

    if (endermanLifetime >= maxLifetime) {
      playRandomHurtSound();
      playTeleportSound(false); // Disappear sound
      endermanVisible = false;
      endermanLifetime = 0.0f;
      currentFrame = 0;
      hoverTimer = 0.0f;
      endermanOpacity = 0.0f;

      if (stareSound &&
          stareSound->getStatus() == sf::SoundSource::Status::Playing) {
        stareSound->stop();
      }
    }
  } else {
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
      spawnEnderman();
      spawnTimer = 0.0f;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<> dis(10.0f, 20.0f);
      spawnInterval = static_cast<float>(dis(gen));
    }
  }
}

void EndermanMode::updateEndermanAnimation(float deltaTime) {
  animationTimer += deltaTime;
  if (animationTimer >= frameTime) {
    currentFrame = (currentFrame + 1) % ENDERMAN_FRAMES;
    animationTimer = 0.0f;

    int col = currentFrame % ENDERMAN_COLS;
    int row = currentFrame / ENDERMAN_COLS;

    if (endermanSprite) {
      endermanSprite->setTextureRect(
        sf::IntRect({col * ENDERMAN_FRAME_WIDTH, row * ENDERMAN_FRAME_HEIGHT},
                    {ENDERMAN_FRAME_WIDTH, ENDERMAN_FRAME_HEIGHT}));
    }
  }
}

void EndermanMode::updateJumpscareAnimation(float deltaTime) {
  jumpscareTimer += deltaTime;

  if (jumpscareTimer >= jumpscareFrameTime) {
    jumpscareFrame++;
    jumpscareTimer = 0.0f;

    if (jumpscareFrame >= JUMPSCARE_TOTAL_FRAMES) {
      playerLost = true;
      return;
    }

    int col = jumpscareFrame % JUMPSCARE_COLS;
    int row = jumpscareFrame / JUMPSCARE_COLS;

    if (jumpscareSprite) {
      jumpscareSprite->setTextureRect(sf::IntRect(
        {col * JUMPSCARE_FRAME_WIDTH, row * JUMPSCARE_FRAME_HEIGHT},
        {JUMPSCARE_FRAME_WIDTH, JUMPSCARE_FRAME_HEIGHT}));
    }
  }
}

void EndermanMode::spawnEnderman() {
  endermanVisible = true;
  endermanLifetime = 0.0f;
  currentFrame = 0;
  hoverTimer = 0.0f;

  // Play IDLE sound on spawn
  playRandomIdleSound();
  playTeleportSound(true); // Appear sound

  // Start visible (ghostly) so player can see it
  // 0.4 opacity = ~100 alpha
  endermanOpacity = 0.4f;

  needsRescaling =
      true; // Defer position/scaling to draw() where we have window

  endermanSprite.emplace(endermanTexture);
  endermanSprite->setTextureRect(
    sf::IntRect({0, 0}, {ENDERMAN_FRAME_WIDTH, ENDERMAN_FRAME_HEIGHT}));

  // Set origin to center
  endermanSprite->setOrigin(
    {ENDERMAN_FRAME_WIDTH / 2.0f, ENDERMAN_FRAME_HEIGHT / 2.0f});

  // Temporarily set position to 0,0 - will be fixed in draw()
  endermanSprite->setPosition({0.0f, 0.0f});

  // Start with appropriate opacity
  auto alpha = static_cast<std::uint8_t>(endermanOpacity * 255.0f);
  endermanSprite->setColor(sf::Color(255, 255, 255, alpha));
}

void EndermanMode::triggerJumpscare() {
  jumpscareActive = true;
  endermanVisible = false;
  jumpscareFrame = 0;
  jumpscareTimer = 0.0f;

  // Reset stare sound
  if (stareSound)
    stareSound->stop();

  jumpscareSprite.emplace(jumpscareTexture);
  jumpscareSprite->setTextureRect(
    sf::IntRect({0, 0}, {JUMPSCARE_FRAME_WIDTH, JUMPSCARE_FRAME_HEIGHT}));
  jumpscareSprite->setPosition({0, 0});

  jumpscareSound.emplace(jumpscareSoundBuffer);
  jumpscareSound->setVolume(100.0f);
  jumpscareSound->play();

  // Allow animation to finish before setting game over state via playerLost logic
}

bool EndermanMode::isMouseOverEnderman(const sf::RenderWindow &window) const {
  if (!endermanSprite) {
    return false;
  }

  sf::Vector2i mousePosPixel = sf::Mouse::getPosition(window);
  sf::Vector2f mousePosWorld = window.mapPixelToCoords(mousePosPixel);

  // Transform mouse position to sprite's local coordinate system
  // The Inverse Transform takes into account Position, Rotation, Scale, Origin
  sf::Transform inverseTransform = endermanSprite->getInverseTransform();
  sf::Vector2f localMouse = inverseTransform.transformPoint(mousePosWorld);

  // Check each hitbox
  return std::any_of(ENDERMAN_HITBOXES.begin(), ENDERMAN_HITBOXES.end(),
                     [&](const Hitbox &box) {
                       float cx = box.x + box.w / 2.0f;
                       float cy = box.y + box.h / 2.0f;

                       // If rotation is 0, simple AABB check
                       if (std::abs(box.r) < 0.1f) {
                         sf::FloatRect rect({box.x, box.y}, {box.w, box.h});
                         return rect.contains(localMouse);
                       }

                       // For rotated boxes
                       float rad = -box.r * static_cast<float>(M_PI) / 180.0f;
                       float s = std::sin(rad);
                       float c = std::cos(rad);

                       float dx = localMouse.x - cx;
                       float dy = localMouse.y - cy;

                       float rx = dx * c - dy * s;
                       float ry = dx * s + dy * c;

                       return std::abs(rx) <= box.w / 2.0f &&
                              std::abs(ry) <= box.h / 2.0f;
                     });
}

bool EndermanMode::handleInput(const sf::Event &event,
                               const sf::RenderWindow &window) {
  if (jumpscareActive) {
    return true;
  }

  return GameModeDecorator::handleInput(event, window);
}

void EndermanMode::updateStareSound(bool isHovering, float deltaTime) {
  if (!stareSound)
    return;

  if (isHovering) {
    // Continue or start playing
    if (stareSound->getStatus() != sf::SoundSource::Status::Playing) {
      stareSound->play();
    }
  } else {
    if (stareSound->getStatus() == sf::SoundSource::Status::Playing) {
      stareSound->pause(); // Pause playback
      // Rewind mechanics: if sound has progress, slowly move offset back
      sf::Time currentOffset = stareSound->getPlayingOffset();
      if (currentOffset.asSeconds() > 0.0f) {
        // Rewind speed factor: Increased to 2.0f to ensure it fully rewinds
        // during fade out
        float rewindAmount = deltaTime * 2.0f;
        sf::Time newOffset = sf::seconds(
          std::max(0.0f, currentOffset.asSeconds() - rewindAmount));
        stareSound->setPlayingOffset(newOffset);
      }
    }
  }
}

void EndermanMode::playRandomIdleSound() {
  if (idleBuffers.empty())
    return;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,
                                      static_cast<int>(idleBuffers.size()) - 1);

  int index = dis(gen);
  idleSound.emplace(idleBuffers[index]);
  idleSound->setVolume(80.0f);
  idleSound->play();
}

void EndermanMode::playRandomHurtSound() {
  if (hurtBuffers.empty())
    return;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,
                                      static_cast<int>(hurtBuffers.size()) - 1);

  int index = dis(gen);
  hurtSound.emplace(hurtBuffers[index]);
  hurtSound->setVolume(90.0f);
  hurtSound->play();
}

void EndermanMode::draw(sf::RenderWindow &window) const {
  // Cache window reference for hover checks in update()
  const_cast<EndermanMode*>(this)->cachedWindow = &window;
  
  if (endermanVisible && endermanSprite) {
    auto *mutableThis = const_cast<EndermanMode *>(this);

    // Apply scaling and positioning
    if (mutableThis->needsRescaling) {
      mutableThis->needsRescaling = false;

      float screenW = static_cast<float>(window.getSize().x);
      float screenH = static_cast<float>(window.getSize().y);

      float targetHeight = screenH * 0.7f;
      float scaleFactor = targetHeight / ENDERMAN_FRAME_HEIGHT;
      mutableThis->endermanSprite->setScale({scaleFactor, scaleFactor});

      float marginX = ENDERMAN_FRAME_WIDTH * scaleFactor * 0.6f;
      float marginY = ENDERMAN_FRAME_HEIGHT * scaleFactor * 0.6f;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<> disX(marginX, screenW - marginX);
      std::uniform_real_distribution<> disY(marginY, screenH - marginY);

      mutableThis->endermanSprite->setPosition(
        {static_cast<float>(disX(gen)), static_cast<float>(disY(gen))});
    }

    // Set color based on current opacity (updated in update(), not here)
    auto alpha = static_cast<std::uint8_t>(endermanOpacity * 255.0f);
    mutableThis->endermanSprite->setColor(sf::Color(255, 255, 255, alpha));

    // Draw Enderman before the darkness
    window.draw(*endermanSprite);
  }

  // Draw particles (same layer as Enderman)
  const_cast<EndermanMode*>(this)->portalSystem.draw(window);

  // Draw the actual game (and Torch darkness if active)
  GameModeDecorator::draw(window);

  // Draw Jumpscare ON TOP of everything
  if (jumpscareActive && jumpscareSprite) {
    // Draw jumpscare as transparent overlay
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());

    sf::Sprite centeredJumpscare = *jumpscareSprite;
    sf::FloatRect bounds = centeredJumpscare.getLocalBounds();

    float scaleX = static_cast<float>(window.getSize().x) / bounds.size.x;
    float scaleY = static_cast<float>(window.getSize().y) / bounds.size.y;
    float scale = std::max(scaleX, scaleY);

    centeredJumpscare.setScale({scale, scale});
    centeredJumpscare.setPosition(
      {
        (static_cast<float>(window.getSize().x) - bounds.size.x * scale) /
        2.0f,
        (static_cast<float>(window.getSize().y) - bounds.size.y * scale) /
        2.0f
      });

    window.draw(centeredJumpscare);
    window.setView(originalView);
  }
}

[[nodiscard]] bool EndermanMode::isLost() const {
  if (playerLost) {
    return true;
  }
  return GameModeDecorator::isLost();
}

[[nodiscard]] std::unique_ptr<GameMode> EndermanMode::clone() const {
  auto clonedWrapped = wrappedMode ? wrappedMode->clone() : nullptr;
  return std::make_unique<EndermanMode>(std::move(clonedWrapped));
}

void EndermanMode::playTeleportSound(bool appear) {
    if (appear) {
        teleportSound.emplace(teleport1Buffer);
    } else {
        teleportSound.emplace(teleport2Buffer);
    }
    teleportSound->setVolume(100.0f);
    teleportSound->play();
}
