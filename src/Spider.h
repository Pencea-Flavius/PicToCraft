#ifndef OOP_SPIDER_H
#define OOP_SPIDER_H

#include <SFML/Graphics.hpp>

class Spider {
public:
  enum class State { Idle, Walking, Dying, Dead };

  Spider(sf::Vector2f startPos, const sf::Texture &walkTex,
         const sf::Texture &idleTex, const sf::Texture &deathTex,
         float scale = 0.3f);

  void update(float dt, const sf::Vector2u &windowSize);

  void draw(sf::RenderWindow &window) const;

  bool contains(sf::Vector2f point) const;

  [[nodiscard]] sf::Vector2f getPosition() const {
    return sprite.getPosition();
  }

  void die();

  [[nodiscard]] bool isDead() const { return state == State::Dead; }
  [[nodiscard]] bool isDying() const { return state == State::Dying; }

  void setTarget(sf::Vector2f pos, bool isRow, int line, int index);

  void clearTarget(); // Added clearTarget
  [[nodiscard]] bool hasActiveTarget() const { return hasTarget; }
  [[nodiscard]] sf::Vector2f getTargetPos() const { return targetPos; }
  [[nodiscard]] bool getIsRowTarget() const { return isRowTarget; }
  [[nodiscard]] int getTargetLine() const { return targetLine; }
  [[nodiscard]] int getTargetIndex() const { return targetIndex; }

private:
  sf::Sprite sprite;
  const sf::Texture *walkTexture;
  const sf::Texture *idleTexture;
  const sf::Texture *deathTexture;

  State state;
  sf::Vector2f velocity;
  float stateTimer;
  float scale; // Added scale member
  float animationTimer;
  int currentFrame;
  int numFrames;
  float frameTime;

  // Target info
  bool hasTarget = false;
  sf::Vector2f targetPos;
  bool isRowTarget = false;
  int targetLine = -1;
  int targetIndex = -1;

  void changeState(State newState);

  void updateAnimation(float dt);

  void updateRotation();
};

#endif // OOP_SPIDER_H