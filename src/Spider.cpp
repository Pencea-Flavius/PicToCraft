#include "Spider.h"
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
    sprite.setRotation(
        sf::degrees(angle - 90.0f)); // Sprite faces down (90 deg)
  }
}

Spider::Spider(sf::Vector2f startPos, const sf::Texture &walkTex,
               const sf::Texture &idleTex, const sf::Texture &deathTex,
               const sf::SoundBuffer *deathSnd,
               const std::vector<sf::SoundBuffer> *idleSnds,
               const std::vector<sf::SoundBuffer> *stepSnds, float scale)
    : sprite(walkTex), walkTexture(&walkTex), idleTexture(&idleTex),
      deathTexture(&deathTex), deathSoundBuffer(deathSnd),
      idleSoundBuffers(idleSnds), stepSoundBuffers(stepSnds),
      state(State::Walking), stateTimer(randomFloat(2.0f, 5.0f)), scale(scale),
      animationTimer(0.0f), currentFrame(0), numFrames(1), frameTime(0.03f) {
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

    // Start walking sound
    if (stepSoundBuffers && !stepSoundBuffers->empty()) {
      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0, stepSoundBuffers->size() - 1);
      
      currentStepSoundIndex = dis(gen);
      audioSource.emplace((*stepSoundBuffers)[currentStepSoundIndex]);
      audioSource->setVolume(currentVolume);
      audioSource->play();

      std::uniform_real_distribution<float> timeDis(1.0f, 2.0f);
      stepTimer = timeDis(gen);
    }
  } else if (state == State::Idle) {
    sprite.setTexture(*idleTexture);
    stateTimer = randomFloat(1.0f, 3.0f);
    velocity = {0.f, 0.f};
    numFrames = 50; // 5 cols * 10 rows
    
    // Stop walking sound if playing
    if (audioSource.has_value() && audioSource->getStatus() == sf::Sound::Status::Playing) {
        audioSource->stop();
    }
    idleSoundTimer = 0.0f; // Reset idle timer
  } else {          // State::Dying
    sprite.setTexture(*deathTexture);
    stateTimer = 100.0f; // Long enough to finish animation
    velocity = {0.f, 0.f};
    numFrames = 20; // 5 cols * 4 rows
    
    // Play death sound
    if (deathSoundBuffer) {
        audioSource.emplace(*deathSoundBuffer);
        audioSource->setVolume(currentVolume);
        audioSource->play();
    }
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
        return;
      }
    } else {
      currentFrame %= numFrames;
    }
  }

  updateAnimation(dt);

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
        stateTimer = randomFloat(1.0f, 2.9f);

        // Random movement
        float angle = randomFloat(0.0f, 360.0f) * 3.14159f / 180.f;
        sf::Vector2f dir = {std::cos(angle), std::sin(angle)};
        velocity = dir * 100.0f;
        float rotationAngle =
            std::atan2(velocity.y, velocity.x) * 180.f / 3.14159f;
        sprite.setRotation(sf::degrees(rotationAngle - 90.f));
      } else {
        changeState(State::Idle);
        stateTimer = randomFloat(0.5f, 1.4f);
        velocity = {0.f, 0.f};
      }
    }

    if (state == State::Walking) {
      sprite.move(velocity * dt);

      sf::Vector2f pos = sprite.getPosition();
      if (pos.x < 0 || pos.x > static_cast<float>(windowSize.x) || pos.y < 0 ||
          pos.y > static_cast<float>(windowSize.y)) {
        velocity = -velocity;
        sprite.setRotation(
            sf::degrees(sprite.getRotation().asDegrees() + 180.f));
      }
    }
    }

  // Audio update
  if (state == State::Walking) {
      stepTimer -= dt;
      if (stepTimer <= 0.0f && stepSoundBuffers && !stepSoundBuffers->empty()) {
          // Play random step sound
          if (!audioSource.has_value() || audioSource->getStatus() != sf::Sound::Status::Playing) {
              static std::random_device rd;
              static std::mt19937 gen(rd());
              std::uniform_int_distribution<> dis(0, stepSoundBuffers->size() - 1);
              std::uniform_real_distribution<float> timeDis(1.5f, 3.0f); // "Rarer" - delay between steps
              
              currentStepSoundIndex = dis(gen);
              audioSource.emplace((*stepSoundBuffers)[currentStepSoundIndex]);
              audioSource->setVolume(currentVolume);
              audioSource->play();
              
              stepTimer = timeDis(gen);
          }
      }
  } else if (state == State::Idle) {
      if ((!audioSource.has_value() || audioSource->getStatus() == sf::Sound::Status::Stopped) && idleSoundBuffers && !idleSoundBuffers->empty()) {
          static std::random_device rd;
          static std::mt19937 gen(rd());
          std::uniform_int_distribution<> dis(0, 300);
          
          if (dis(gen) == 0) {
             std::uniform_int_distribution<> idxDis(0, idleSoundBuffers->size() - 1);
             int idx = idxDis(gen);
             audioSource.emplace((*idleSoundBuffers)[idx]);
             audioSource->setVolume(currentVolume);
             audioSource->play();
          }
      }
  }
}

void Spider::updateAnimation(float dt) {

  int cols = 5;
  int frameWidth = 350;
  int frameHeight = 471;

  int col = currentFrame % cols;
  int row = currentFrame / cols;

  sf::IntRect rect({col * frameWidth, row * frameHeight},
                   {frameWidth, frameHeight});
  sprite.setTextureRect(rect);

  // Ensure scale is positive
  sprite.setScale({scale, scale});
}

void Spider::draw(sf::RenderWindow &window) const { window.draw(sprite); }

bool Spider::contains(sf::Vector2f point) const {
  // Transform the point to the sprite's local coordinate system
  sf::Transform inverseTransform = sprite.getInverseTransform();
  sf::Vector2f localPoint = inverseTransform.transformPoint(point);

  // Define the local hitbox
  sf::FloatRect localHitbox({75.f, 220.f}, {200.f, 200.f});

  return localHitbox.contains(localPoint);
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
