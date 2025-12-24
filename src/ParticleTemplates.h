#ifndef PARTICLE_TEMPLATES_H
#define PARTICLE_TEMPLATES_H

#include <SFML/Graphics.hpp>
#include <vector>

namespace ParticleUtils {
    float randomFloat(float min, float max);
    
    template <typename T>
    T lerp(T start, T end, float t) {
        return start + static_cast<T>((end - start) * t);
    }
}

struct TemplateParticle {
    sf::Vector2f position{};
    sf::Vector2f velocity{};
    float lifetime{0.f};
    float maxLifetime{1.f};
    sf::Color color{sf::Color::White};
    float size{10.f};
    float rotation{0.f};
    float angularVelocity{0.f};
};

struct PotionSplashTrait {
    static void init(TemplateParticle& p, sf::Color baseColor, float scale);
    static void update(TemplateParticle& p, float dt);
    static sf::BlendMode getBlendMode();
};

struct WitchMagicTrait {
    static void init(TemplateParticle& p, sf::Color unused, float scale);
    static void update(TemplateParticle& p, float dt);
    static sf::BlendMode getBlendMode();
};

template <typename Trait>
class TemplateParticleSystem {
    std::vector<TemplateParticle> particles;
    sf::VertexArray vertices;
    std::vector<const sf::Texture*> textures; // Animated frames

public:
    TemplateParticleSystem() : vertices(sf::PrimitiveType::Triangles) {} 
    
    void addTexture(const sf::Texture* tex) { textures.push_back(tex); }
    void clearTextures() { textures.clear(); }

    void emit(sf::Vector2f position, int count, sf::Color color = sf::Color::White, float scale = 1.0f) {
        for(int i=0; i<count; ++i) {
            TemplateParticle p;
            p.position = position;
            Trait::init(p, color, scale);
            particles.push_back(p);
        }
    }

    void update(float dt) {
        for(auto it = particles.begin(); it != particles.end(); ) {
            it->lifetime -= dt;
            if(it->lifetime <= 0) {
                it = particles.erase(it);
            } else {
                it->position += it->velocity * dt;
                Trait::update(*it, dt);
                ++it;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        if(textures.empty()) return;

        sf::RenderStates states;
        states.blendMode = Trait::getBlendMode();

        for(size_t i = 0; i < textures.size(); ++i) {
             const sf::Texture* tex = textures[i];
             if(!tex) continue;
             sf::Vector2f texSize = static_cast<sf::Vector2f>(tex->getSize());
             
             vertices.clear();
             
             // Collect particles for this frame
             for(const auto& p : particles) {
                 float ratio = p.lifetime / p.maxLifetime;
                 // Index: 0 to N-1.
                 // ratio goes from 1.0 (start) to 0.0 (end)
                 // We want index to go from (textures.size() - 1) down to 0.
                 int frameIndex = static_cast<int>(ratio * (static_cast<float>(textures.size()) - 0.0001f)); // Use a small epsilon to ensure 1.0 maps to max index
                 
                 // Clamp frameIndex to valid range [0, textures.size() - 1]
                 if(frameIndex >= static_cast<int>(textures.size())) frameIndex = static_cast<int>(textures.size()) - 1;
                 if(frameIndex < 0) frameIndex = 0;
                 
                 if (frameIndex == static_cast<int>(i)) {
                    sf::Vector2f pos = p.position;
                    float s = p.size;
                    sf::Color c = p.color;
                    
                    vertices.append(sf::Vertex{pos + sf::Vector2f{-s, -s}, c, sf::Vector2f{0.f, 0.f}});
                    vertices.append(sf::Vertex{pos + sf::Vector2f{s, -s}, c, sf::Vector2f{texSize.x, 0.f}});
                    vertices.append(sf::Vertex{pos + sf::Vector2f{-s, s}, c, sf::Vector2f{0.f, texSize.y}});
                    
                    vertices.append(sf::Vertex{pos + sf::Vector2f{-s, s}, c, sf::Vector2f{0.f, texSize.y}});
                    vertices.append(sf::Vertex{pos + sf::Vector2f{s, -s}, c, sf::Vector2f{texSize.x, 0.f}});
                    vertices.append(sf::Vertex{pos + sf::Vector2f{s, s}, c, sf::Vector2f{texSize.x, texSize.y}});
                 }
             }
             
             if(vertices.getVertexCount() > 0) {
                 states.texture = tex;
                 window.draw(vertices, states);
             }
        }
    }
};

#endif // PARTICLE_TEMPLATES_H
