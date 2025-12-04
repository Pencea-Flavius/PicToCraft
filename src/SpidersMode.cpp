#include "SpidersMode.h"
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
    std::cerr << "Failed to load spider_walk.png from "
              << cwd / "assets/enemy/spider_walk.png" << std::endl;
  }
  if (!idleTexture.loadFromFile("assets/enemy/spider_idle.png")) {
    std::cerr << "Failed to load spider_idle.png" << std::endl;
  }
  if (!deathTexture.loadFromFile("assets/enemy/spider_death.png")) {
    std::cerr << "Failed to load spider_death.png" << std::endl;
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

  // Assign target if grid/renderer available
  if (grid && renderer) {
    // Retarget logic removed as per user request
    // Spiders will now roam freely after their first target

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
          spider.clearTarget(); // Don't die, just clear target
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
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
    damageTimer += deltaTime;
    if (damageTimer >= 0.1f) {
      // Damage every 0.1s
      damageTimer = 0.0f;
      if (renderer) {
        // Use last known mouse position
        renderer->handleHintClick(lastMousePos);
      }
    }
  } else {
    damageTimer = 0.0f;
    if (grid) {
      grid->healWebs();
    }
  }

  spiders.erase(std::remove_if(spiders.begin(), spiders.end(),
                               [](const Spider &s) { return s.isDead(); }),
                spiders.end());
}

void SpidersMode::draw(sf::RenderWindow &window) const {
  GameModeDecorator::draw(window);

  const_cast<SpidersMode *>(this)->windowSize = window.getSize();

  for (auto &spider : spiders) {
    auto &s = const_cast<Spider &>(spider);
    s.draw(window);
  }
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
          spider.die();
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

  spiders.emplace_back(sf::Vector2f(x, y), walkTexture, idleTexture,
                       deathTexture, spiderScale);

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
