#include "NinePatch.h"
#include <algorithm>
#include <cmath>

NinePatch::NinePatch()
    : m_texture(nullptr), m_cornerSize(0), m_edgeThickness(0),
      m_size(0.f, 0.f) {
}

NinePatch::NinePatch(const sf::Texture &texture, int cornerSize,
                     int edgeThickness)
    : m_texture(&texture), m_cornerSize(cornerSize),
      m_edgeThickness(edgeThickness), m_size(0.f, 0.f) {
    setupSprites();
}

void NinePatch::setTexture(const sf::Texture &texture) {
    m_texture = &texture;
    setupSprites();
    updateLayout();
}

void NinePatch::setCornerSize(int size) {
    m_cornerSize = size;
    setupSprites();
    updateLayout();
}

void NinePatch::setSize(float width, float height) {
    m_size = {width, height};
    updateLayout();
}

sf::Vector2f NinePatch::getSize() const { return m_size; }

void NinePatch::setColor(const sf::Color &color) {
    for (auto &sprite: m_sprites) {
        sprite.setColor(color);
    }
}

void NinePatch::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();
    for (const auto &sprite: m_sprites) {
        target.draw(sprite, states);
    }
}

void NinePatch::setupSprites() {
    if (!m_texture || m_cornerSize <= 0)
        return;

    m_sprites.clear();
    m_sprites.reserve(9);

    auto texSize = m_texture->getSize();
    int texWidth = static_cast<int>(texSize.x);
    int texHeight = static_cast<int>(texSize.y);

    int edgeWidth = texWidth - 2 * m_cornerSize;
    int edgeHeight = texHeight - 2 * m_cornerSize;

    // Ensure texture is large enough
    if (edgeWidth < 0 || edgeHeight < 0)
        return;

    m_rects = {
        {
            sf::IntRect({0, 0}, {m_cornerSize, m_cornerSize}),
            sf::IntRect({m_cornerSize, 0}, {edgeWidth, m_cornerSize}),
            sf::IntRect({texWidth - m_cornerSize, 0}, {m_cornerSize, m_cornerSize}),
            sf::IntRect({0, m_cornerSize}, {m_cornerSize, edgeHeight}),
            sf::IntRect({m_cornerSize, m_cornerSize}, {edgeWidth, edgeHeight}),
            sf::IntRect({texWidth - m_cornerSize, m_cornerSize},
                        {m_cornerSize, edgeHeight}),
            sf::IntRect({0, texHeight - m_cornerSize}, {m_cornerSize, m_cornerSize}),
            sf::IntRect({m_cornerSize, texHeight - m_cornerSize},
                        {edgeWidth, m_cornerSize}),
            sf::IntRect({texWidth - m_cornerSize, texHeight - m_cornerSize},
                        {m_cornerSize, m_cornerSize}),
        }
    };

    for (int i = 0; i < 9; i++) {
        m_sprites.emplace_back(*m_texture);
        m_sprites[i].setTextureRect(m_rects[i]);
    }
}

void NinePatch::setPatchScale(float scale) {
    m_patchScale = scale;
    updateLayout();
}

void NinePatch::setPixelSnapping(bool enabled) {
    m_pixelSnapping = enabled;
    updateLayout();
}

void NinePatch::updateLayout() {
    if (m_sprites.empty())
        return;

    float targetWidth = m_size.x;
    float targetHeight = m_size.y;

    int texWidth = static_cast<int>(m_texture->getSize().x);
    int texHeight = static_cast<int>(m_texture->getSize().y);
    int edgeWidth = texWidth - 2 * m_cornerSize;
    int edgeHeight = texHeight - 2 * m_cornerSize;
    float baseScale =
            (m_edgeThickness > 0) ? (float) m_edgeThickness / m_cornerSize : 1.0f;
    float finalPatchScale = baseScale * m_patchScale;

    float scaledCornerSize = m_cornerSize * finalPatchScale;

    // Positions
    float x[4] = {
        0.f, scaledCornerSize, targetWidth - scaledCornerSize,
        targetWidth
    };
    float y[4] = {
        0.f, scaledCornerSize, targetHeight - scaledCornerSize,
        targetHeight
    };

    // If target size is too small, we might overlap.
    if (targetWidth < 2 * scaledCornerSize) {
        float mid = targetWidth / 2.f;
        x[1] = mid;
        x[2] = mid;
    }
    if (targetHeight < 2 * scaledCornerSize) {
        float mid = targetHeight / 2.f;
        y[1] = mid;
        y[2] = mid;
    }

    if (m_pixelSnapping) {
        for (int i = 0; i < 4; ++i) {
            x[i] = std::round(x[i]);
            y[i] = std::round(y[i]);
        }
    }

    // Set positions and scales for each sprite
    // 0: Top-Left
    m_sprites[0].setPosition({x[0], y[0]});
    m_sprites[0].setScale({finalPatchScale, finalPatchScale});

    // 1: Top
    m_sprites[1].setPosition({x[1], y[0]});
    m_sprites[1].setScale({(x[2] - x[1]) / edgeWidth, finalPatchScale});

    // 2: Top-Right
    m_sprites[2].setPosition({x[2], y[0]});
    m_sprites[2].setScale({finalPatchScale, finalPatchScale});

    // 3: Left
    m_sprites[3].setPosition({x[0], y[1]});
    m_sprites[3].setScale({finalPatchScale, (y[2] - y[1]) / edgeHeight});

    // 4: Center
    m_sprites[4].setPosition({x[1], y[1]});
    m_sprites[4].setScale(
        {(x[2] - x[1]) / edgeWidth, (y[2] - y[1]) / edgeHeight});

    // 5: Right
    m_sprites[5].setPosition({x[2], y[1]});
    m_sprites[5].setScale({finalPatchScale, (y[2] - y[1]) / edgeHeight});

    // 6: Bottom-Left
    m_sprites[6].setPosition({x[0], y[2]});
    m_sprites[6].setScale({finalPatchScale, finalPatchScale});

    // 7: Bottom
    m_sprites[7].setPosition({x[1], y[2]});
    m_sprites[7].setScale({(x[2] - x[1]) / edgeWidth, finalPatchScale});

    // 8: Bottom-Right
    m_sprites[8].setPosition({x[2], y[2]});
    m_sprites[8].setScale({finalPatchScale, finalPatchScale});
}