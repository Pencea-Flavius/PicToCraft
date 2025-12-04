#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
  sf::Vector2f position{};
  sf::Vector2f velocity{};
  float lifetime{0.f};
  float maxLifetime{0.f};
  sf::Color color{};
  float size{0.f};
};

enum class ParticleType { Fire, Smoke };

class ParticleSystem {
public:
  ParticleSystem();

  void emit(sf::Vector2f position, ParticleType type);

  void update(float dt);

  void draw(sf::RenderWindow &window);

private:
  std::vector<Particle> particles;
  sf::VertexArray vertices;
};

#endif // PARTICLE_SYSTEM_H