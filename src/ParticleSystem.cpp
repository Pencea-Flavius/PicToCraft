#include "ParticleSystem.h"
#include <cmath>
#include <cstdint>
#include <random>

static float randomFloat(float min, float max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(min, max);
  return dis(gen);
}

ParticleSystem::ParticleSystem() : vertices(sf::PrimitiveType::Triangles) {}

void ParticleSystem::emit(sf::Vector2f position, ParticleType type) {
  Particle p;
  p.position = position;

  if (type == ParticleType::Fire) {
    // Narrower cone for torch flame (upwards)
    float angle = randomFloat(-3.14159f / 8.0f, 3.14159f / 8.0f) -
                  3.14159f / 2.0f; // +/- 22.5 degrees
    float speed = randomFloat(30.0f, 80.0f);
    p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};

    p.maxLifetime = randomFloat(0.3f, 0.7f);
    p.lifetime = p.maxLifetime;

    // Fire colors
    int r = 255;
    int g = static_cast<int>(randomFloat(50, 150));
    int b = 0;
    p.color = sf::Color(r, g, b, 255);

    p.size = randomFloat(3.0f, 6.0f); // Slightly larger particles
  } else {
    // Smoke
    // Slower, more spread out
    float angle = randomFloat(0.0f, 2.0f * 3.14159f);
    float speed = randomFloat(20.0f, 60.0f);
    // Bias upwards
    p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed - 30.0f};

    p.maxLifetime = randomFloat(1.0f, 2.0f);
    p.lifetime = p.maxLifetime;

    // Grey smoke
    int grey = static_cast<int>(randomFloat(20, 50)); // Darker grey
    p.color = sf::Color(grey, grey, grey, 100);       // Semi-transparent

    p.size = randomFloat(4.0f, 8.0f);
  }

  particles.push_back(p);
}

void ParticleSystem::update(float dt) {
  for (auto it = particles.begin(); it != particles.end();) {
    it->lifetime -= dt;
    if (it->lifetime <= 0) {
      it = particles.erase(it);
    } else {
      it->position += it->velocity * dt;

      // Fade out
      float ratio = it->lifetime / it->maxLifetime;
      it->color.a = static_cast<std::uint8_t>(ratio * 255);

      // Shrink
      it->size = std::max(0.0f, it->size - dt * 2.0f);

      ++it;
    }
  }
}

void ParticleSystem::draw(sf::RenderWindow &window) {
  vertices.clear();

  std::size_t start = vertices.getVertexCount();
  vertices.resize(start + particles.size() * 6);

  std::size_t idx = start;
  for (const auto &p : particles) {
    // Create a small quad for each particle
    sf::Vector2f pos = p.position;
    float size = p.size;
    sf::Color col = p.color;

    vertices[idx++] = sf::Vertex{pos + sf::Vector2f(-size, -size), col};
    vertices[idx++] = sf::Vertex{pos + sf::Vector2f(size, -size), col};
    vertices[idx++] = sf::Vertex{pos + sf::Vector2f(size, size), col};

    vertices[idx++] = sf::Vertex{pos + sf::Vector2f(-size, -size), col};
    vertices[idx++] = sf::Vertex{pos + sf::Vector2f(size, size), col};
    vertices[idx++] = sf::Vertex{pos + sf::Vector2f(-size, size), col};
  }

  // Use additive blending for fire effect
  sf::RenderStates states;
  states.blendMode = sf::BlendAdd;
  window.draw(vertices, states);
}
