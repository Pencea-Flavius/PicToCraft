#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::Color color;
    float size;
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