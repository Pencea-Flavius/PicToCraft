#include "SpidersMode.h"
#include "AlchemyMode.h"
#include "Exceptions.h"
#include "Grid.h"
#include "GridRenderer.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <random>

// Helper for random numbers
static float randomFloat(float max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, max);
  return dis(gen);
}

static int getRandomEdge() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 3);
  return dis(gen);
}

// SpidersMode Implementation
SpidersMode::SpidersMode(std::unique_ptr<GameMode> mode)
    : GameModeDecorator(std::move(mode)), spawnTimer(0.0f), damageTimer(0.0f) {
  std::filesystem::path cwd = std::filesystem::current_path();
  std::cout << "Current working directory: " << cwd << std::endl;

  if (!walkTexture.loadFromFile("assets/enemy/spider_walk.png")) {
    throw AssetLoadException("assets/enemy/spider_walk.png", "Texture");
  }
  if (!idleTexture.loadFromFile("assets/enemy/spider_idle.png")) {
    throw AssetLoadException("assets/enemy/spider_idle.png", "Texture");
  }
  if (!deathTexture.loadFromFile("assets/enemy/spider_death.png")) {
    throw AssetLoadException("assets/enemy/spider_death.png", "Texture");
  }

  if (!deathBuffer.loadFromFile("assets/sound/Spider_death.ogg")) {
    throw AssetLoadException("assets/sound/Spider_death.ogg", "Sound");
  }

  idleBuffers.reserve(4);
  for (int i = 1; i <= 4; ++i) {
    sf::SoundBuffer buf;
    if (buf.loadFromFile("assets/sound/Spider_idle" + std::to_string(i) +
                         ".ogg")) {
      idleBuffers.push_back(buf);
    } else {
      throw AssetLoadException(
          "assets/sound/Spider_idle" + std::to_string(i) + ".ogg", "Sound");
    }
  }

  stepBuffers.reserve(4);
  for (int i = 1; i <= 4; ++i) {
    sf::SoundBuffer buf;
    if (buf.loadFromFile("assets/sound/Spider_step" + std::to_string(i) +
                         ".ogg")) {
      stepBuffers.push_back(buf);
    } else {
      throw AssetLoadException(
          "assets/sound/Spider_step" + std::to_string(i) + ".ogg", "Sound");
    }
  }

  // Web sounds
  if (!brokenWebBuffer.loadFromFile("assets/sound/break.ogg")) {
    throw AssetLoadException("assets/sound/break.ogg", "Sound");
  }

  hitWebBuffers.reserve(5);
  for (int i = 1; i <= 5; ++i) {
    sf::SoundBuffer buf;
    if (buf.loadFromFile("assets/sound/Stone_hit" + std::to_string(i) +
                         ".ogg")) {
      hitWebBuffers.push_back(buf);
    } else {
      throw AssetLoadException(
          "assets/sound/Stone_hit" + std::to_string(i) + ".ogg", "Sound");
    }
  }
}

void SpidersMode::setGrid(Grid *g) {
  grid = g;
  if (wrappedMode)
    wrappedMode->setGrid(g);
}

void SpidersMode::setRenderer(const GridRenderer *r) {
  renderer = r;
  if (wrappedMode)
    wrappedMode->setRenderer(r);
}

void SpidersMode::update(float deltaTime) {
  GameModeDecorator::update(deltaTime);

  // Spawn spiders
  spawnTimer += deltaTime;
  if (spawnTimer >= 5.0f) {
    // Spawn every 5 seconds
    spawnTimer = 0.0f;
    spawnSpider();
  }

  // Update spiders
  if (windowSize.x == 0)
    windowSize = {1280, 720};

  if (grid && renderer) {
    for (auto &spider : spiders) {
      spider.update(deltaTime, windowSize);

      // Check if spider reached target
      if (spider.hasActiveTarget()) {
        float dx = spider.getTargetPos().x - spider.getPosition().x;
        float dy = spider.getTargetPos().y - spider.getPosition().y;
        float distSq = dx * dx + dy * dy;

        if (distSq < 100.0f) {
          // Reached target
          grid->webHint(spider.getIsRowTarget(), spider.getTargetLine(),
                        spider.getTargetIndex());
          spider.clearTarget();
          // Spider will now wander until retargeted
        }
      }
    }
  } else {
    // Just update if no grid
    for (auto &spider : spiders) {
      spider.update(deltaTime, windowSize);
    }
  }
  
  // Update spider health only when Weakness effect changes
  if (grid && grid->getMode()) {
    if (auto* alchemyMode = dynamic_cast<AlchemyMode*>(grid->getMode())) {
      bool hasWeakness = alchemyMode->hasEffect(EffectType::Weakness);
      
      // Only update if weakness state changed
      if (hasWeakness != wasWeaknessActive) {
        wasWeaknessActive = hasWeakness;
        for (auto &spider : spiders) {
          if (!spider.isDying() && !spider.isDead()) {
            spider.setHealth(hasWeakness ? 2 : 1);
          }
        }
      }
    }
  }
  
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
    damageTimer += deltaTime;
    
    // Haste makes damage faster, Mining Fatigue makes it slower
    float damageRate = 0.1f; // Default: damage every 0.1s
    
    // Check for Haste/Mining Fatigue from AlchemyMode
    if (grid && grid->getMode()) {
      if (auto* alchemyMode = dynamic_cast<AlchemyMode*>(grid->getMode())) {
        if (alchemyMode->hasEffect(EffectType::Haste)) {
          damageRate = 0.05f; // Twice as fast
        } else if (alchemyMode->hasEffect(EffectType::MiningFatigue)) {
          damageRate = 0.3f; // 3x slower
        }
      }
    }
    
    if (damageTimer >= damageRate) {
      // Damage web
      damageTimer = 0.0f;
      if (renderer) {
        // Use last known mouse position
        Grid::WebDamageResult result = renderer->handleHintClick(lastMousePos);

        if (result == Grid::WebDamageResult::Destroyed) {
          webAudioSource.emplace(brokenWebBuffer);
          webAudioSource->setVolume(currentVolume);
          webAudioSource->play();
        } else if (result == Grid::WebDamageResult::Damaged) {
          if (!hitWebBuffers.empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, static_cast<int>(hitWebBuffers.size()) - 1);

            // Play random stone hit
            int hitIdx = dis(gen);
            webAudioSource.emplace(hitWebBuffers[hitIdx]);
            webAudioSource->setVolume(currentVolume);
            webAudioSource->play();
          }
        }
      }
    }
  } else {
    damageTimer = 0.0f;
    if (grid) {
      grid->healWebs();
    }
  }

  std::erase_if(spiders, [](const Spider &s) { return s.isDead(); });
}

void SpidersMode::draw(sf::RenderWindow &window) const {
  GameModeDecorator::draw(window);

  const_cast<SpidersMode *>(this)->windowSize = window.getSize();

  for (const auto &spider : spiders) {
    spider.draw(window);
  }
}

void SpidersMode::setSfxVolume(float volume) {
  if (wrappedMode)
    wrappedMode->setSfxVolume(volume);

  // Store volume? Or just set?
  // Spiders need to know volume when they emulate sound inside update
  // But Spider has audioSource.
  // We update existing spiders
  for (auto &spider : spiders) {
    spider.setVolume(volume);
  }

  // Web audio
  if (webAudioSource)
    webAudioSource->setVolume(volume);
  // Also need to store it for future plays?
  // webAudioSource is a single optional.
  currentVolume = volume;
}

bool SpidersMode::handleInput(const sf::Event &event,
                              const sf::RenderWindow &window) {
  // First check wrapped mode
  if (GameModeDecorator::handleInput(event, window))
    return true;

  if (auto m = event.getIf<sf::Event::MouseMoved>()) {
    lastMousePos = m->position;
  }

  if (event.is<sf::Event::MouseButtonPressed>()) {
    if (auto m = event.getIf<sf::Event::MouseButtonPressed>();
        m && m->button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos = window.mapPixelToCoords(m->position);

      // Spider clicking
      for (auto &spider : spiders) {
        if (!spider.isDying() && !spider.isDead() &&
            spider.contains(mousePos)) {
          spider.hit(); // Use hit() instead of die() for Weakness support
          return true; // Consumed
        }
      }
    }
  }
  return false;
}

void SpidersMode::spawnSpider() {
  if (windowSize.x == 0)
    windowSize = {1280, 720};

  float scaleX = static_cast<float>(windowSize.x) / 1920.0f;
  float scaleY = static_cast<float>(windowSize.y) / 1080.0f;
  float baseScale = std::min(scaleX, scaleY);
  float spiderScale = 0.3f * baseScale * 1.5f;

  // Spawn from edges
  // Spawn from edges
  float x = 0.0f;
  float y = 0.0f;
  int edge = getRandomEdge(); // 0: Top, 1: Right, 2: Bottom, 3: Left

  float padding = 50.0f * baseScale; // Spawn slightly outside

  switch (edge) {
  case 0: // Top
    x = randomFloat(static_cast<float>(windowSize.x));
    y = -padding;
    break;
  case 1: // Right
    x = static_cast<float>(windowSize.x) + padding;
    y = randomFloat(static_cast<float>(windowSize.y));
    break;
  case 2: // Bottom
    x = randomFloat(static_cast<float>(windowSize.x));
    y = static_cast<float>(windowSize.y) + padding;
    break;
  case 3: // Left
    x = -padding;
    y = randomFloat(static_cast<float>(windowSize.y));
    break;
  default:
    break;
  }

  Spider newSpider(sf::Vector2f(x, y), walkTexture, idleTexture, deathTexture,
                   &deathBuffer, &idleBuffers, &stepBuffers, spiderScale);
  newSpider.setVolume(currentVolume);
  
  // Set health based on Weakness effect
  if (grid && grid->getMode()) {
    if (auto* alchemyMode = dynamic_cast<AlchemyMode*>(grid->getMode())) {
      if (alchemyMode->hasEffect(EffectType::Weakness)) {
        newSpider.setHealth(2); // Spiders need 2 hits with Weakness
      }
    }
  }
  
  spiders.push_back(std::move(newSpider));
  // Assign target if grid/renderer available
  if (grid && renderer) {
    // Pick random hint
    const auto &hints = grid->get_hints();
    const auto &rowHints = hints.get_row_hints();
    const auto &colHints = hints.get_col_hints();

    bool pickRow = randomFloat(1) > 0.5f; // Changed from randomFloat(0, 1)
    if (rowHints.empty())
      pickRow = false;
    if (colHints.empty() && !pickRow)
      return;

    int line = -1;
    int index = -1;

    // Try to find unwebbed hint
    int attempts = 10;
    while (attempts-- > 0) {
      if (pickRow) {
        line = static_cast<int>(
            randomFloat(static_cast<float>(rowHints.size()) - 0.1f));
        if (rowHints[line].empty())
          continue;
        index = static_cast<int>(
            randomFloat(static_cast<float>(rowHints[line].size()) - 0.1f));
        if (!grid->isHintWebbed(true, line, index))
          break;
      } else {
        line = static_cast<int>(
            randomFloat(static_cast<float>(colHints.size()) - 0.1f));
        if (colHints[line].empty())
          continue;
        index = static_cast<int>(
            randomFloat(static_cast<float>(colHints[line].size()) - 0.1f));
        if (!grid->isHintWebbed(false, line, index))
          break;
      }
    }

    if (line != -1 && index != -1) {
      sf::Vector2f targetPos = renderer->getHintCenter(pickRow, line, index);
      spiders.back().setTarget(targetPos, pickRow, line, index);
    }
  }
}

std::unique_ptr<GameMode> SpidersMode::clone() const {
  auto clone = std::make_unique<SpidersMode>(wrappedMode->clone());
  clone->grid = grid;
  clone->renderer = renderer;
  return clone;
}
