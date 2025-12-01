#ifndef NINEPATCH_H
#define NINEPATCH_H

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

struct NinePatchConfig {
    int cornerSize;
    int edgeThickness;
};

class NinePatch : public sf::Drawable, public sf::Transformable {
public:
    NinePatch();

    NinePatch(const sf::Texture &texture, int cornerSize, int edgeThickness = 0);

    void setTexture(const sf::Texture &texture);

    void setCornerSize(int size);

    void setEdgeThickness(int thickness); // 0 means use cornerSize

    void setSize(float width, float height);

    sf::Vector2f getSize() const;

    void setColor(const sf::Color &color);

    // Sets the scale factor for the patches (corners and edges).
    // This allows for pixel-perfect scaling or adjusting the border thickness.
    void setPatchScale(float scale);

    // Enables or disables pixel snapping for positions and sizes.
    void setPixelSnapping(bool enabled);

private:
    virtual void draw(sf::RenderTarget &target,
                      sf::RenderStates states) const override;

    void updateLayout();

    void setupSprites();

    const sf::Texture *m_texture;
    int m_cornerSize;
    int m_edgeThickness;
    float m_patchScale = 1.0f;
    bool m_pixelSnapping = false;
    sf::Vector2f m_size;

    std::vector<sf::Sprite> m_sprites;
    std::array<sf::IntRect, 9> m_rects;
};

#endif // NINEPATCH_H