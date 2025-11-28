#include "Spider.h"
#include <algorithm>
#include <cmath>
#include <random>

// Helper for random numbers
static float randomFloat(float min, float max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(min, max);
  return dis(gen);
}

// Helper to update rotation based on velocity
void Spider::updateRotation() {
  if (velocity.x != 0.f || velocity.y != 0.f) {
    float angle = std::atan2(velocity.y, velocity.x) * 180.0f / 3.14159f;
    sprite.setRotation(sf::degrees(
      angle - 90.0f)); // Sprite faces down (90 deg), so subtract 90
  }
}

Spider::Spider(sf::Vector2f startPos, const sf::Texture &walkTex,
               const sf::Texture &idleTex, const sf::Texture &deathTex,
               float scale)
  : walkTexture(&walkTex), idleTexture(&idleTex), deathTexture(&deathTex),
    state(State::Walking), stateTimer(randomFloat(2.0f, 5.0f)),
    animationTimer(0.0f), currentFrame(0), numFrames(1), frameTime(0.03f),
    sprite(walkTex), scale(scale) {
  sprite.setPosition(startPos);
  sprite.setScale({scale, scale});

  // Frame size 350x471
  sprite.setOrigin({175.0f, 235.5f});

  // Random velocity
  float angle = randomFloat(0.0f, 2.0f * 3.14159f);
  float speed = randomFloat(50.0f, 100.0f);
  velocity = {std::cos(angle) * speed, std::sin(angle) * speed};

  changeState(State::Walking);
}

void Spider::changeState(State newState) {
  state = newState;
  currentFrame = 0;
  animationTimer = 0.0f;

  if (state == State::Walking) {
    sprite.setTexture(*walkTexture);
    stateTimer = randomFloat(2.0f, 5.0f);
    numFrames = 25; // 5 cols * 5 rows

    // Random new velocity
    float angle = randomFloat(0.0f, 2.0f * 3.14159f);
    float speed = randomFloat(50.0f, 100.0f);
    velocity = {std::cos(angle) * speed, std::sin(angle) * speed};

    updateRotation();
  } else if (state == State::Idle) {
    sprite.setTexture(*idleTexture);
    stateTimer = randomFloat(1.0f, 3.0f);
    velocity = {0.f, 0.f};
    numFrames = 50; // 5 cols * 10 rows
  } else if (state == State::Dying) {
    sprite.setTexture(*deathTexture);
    stateTimer = 100.0f; // Long enough to finish animation
    velocity = {0.f, 0.f};
    numFrames = 20; // 5 cols * 4 rows
  }

  updateAnimation(0.0f);
}

void Spider::update(float dt, const sf::Vector2u &windowSize) {
  if (state == State::Dead)
    return;

  stateTimer -= dt;

  // Update animation frame
  animationTimer += dt;
  if (animationTimer >= frameTime) {
    animationTimer = 0.0f;
    currentFrame++;
    if (state == State::Dying) {
      if (currentFrame >= numFrames) {
        state = State::Dead;
        currentFrame = numFrames - 1;
      }
    } else {
      currentFrame %= numFrames;
    }
  }

  updateAnimation(dt);

  if (state == State::Dead)
    return;
  if (state == State::Dying)
    return;

  if (hasTarget) {
    // Move towards target
    sf::Vector2f currentPos = sprite.getPosition();
    sf::Vector2f dir = targetPos - currentPos;
    float distSq = dir.x * dir.x + dir.y * dir.y;

    if (distSq > 100.0f) {
      float length = std::sqrt(distSq);
      dir /= length;
      velocity = dir * 100.0f; // Speed

      // Update rotation
      float angle = std::atan2(velocity.y, velocity.x) * 180.f / 3.14159f;
      sprite.setRotation(sf::degrees(angle - 90.f));

      sprite.move(velocity * dt);

      if (state != State::Walking) {
        changeState(State::Walking);
      }
    } else {
      // Reached
      sprite.setPosition(targetPos);
      velocity = {0, 0};
      if (state != State::Idle) {
        changeState(State::Idle);
      }
    }
  } else {
    // Random movement logic
    if (stateTimer <= 0) {
      if (state == State::Idle) {
        changeState(State::Walking);
        stateTimer = static_cast<float>(rand() % 20 + 10) / 10.0f;

        sf::Vector2f dir;
        if (hasTarget) {
        } else {
          // Random movement
          float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
          dir = {std::cos(angle), std::sin(angle)};
        }

        velocity = dir * 100.0f;
        float angle = std::atan2(velocity.y, velocity.x) * 180.f / 3.14159f;
        sprite.setRotation(sf::degrees(angle - 90.f));
      } else {
        changeState(State::Idle);
        stateTimer = static_cast<float>(rand() % 10 + 5) / 10.0f;
        velocity = {0.f, 0.f};
      }
    }

    if (state == State::Walking) {
      sprite.move(velocity * dt);

      sf::Vector2f pos = sprite.getPosition();
      if (pos.x < 0 || pos.x > windowSize.x || pos.y < 0 ||
          pos.y > windowSize.y) {
        velocity = -velocity;
        sprite.setRotation(
          sf::degrees(sprite.getRotation().asDegrees() + 180.f));
      }
    }
  }
}

void Spider::updateAnimation(float dt) {
  const sf::Texture &tex = sprite.getTexture();

  int cols = 5;
  int frameWidth = 350;
  int frameHeight = 471;

  int col = currentFrame % cols;
  int row = currentFrame / cols;

  sf::IntRect rect({col * frameWidth, row * frameHeight},
                   {frameWidth, frameHeight});
  sprite.setTextureRect(rect);

  // Ensure scale is positive (no flipping)
  sprite.setScale({scale, scale});
}

void Spider::draw(sf::RenderWindow &window) { window.draw(sprite); }

bool Spider::contains(sf::Vector2f point) const {
  return sprite.getGlobalBounds().contains(point);
}

void Spider::die() {
  if (state != State::Dying && state != State::Dead) {
    changeState(State::Dying);
  }
}

void Spider::setTarget(sf::Vector2f pos, bool isRow, int line, int index) {
  targetPos = pos;
  isRowTarget = isRow;
  targetLine = line;
  targetIndex = index;
  hasTarget = true;
}

void Spider::clearTarget() {
  hasTarget = false;
  changeState(State::Idle); // Go to idle when target is cleared
}
