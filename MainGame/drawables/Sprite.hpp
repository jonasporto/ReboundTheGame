#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Sprite : public sf::Drawable
{
    static sf::Shader& getSpriteShader();
    
    std::shared_ptr<sf::Texture> texture;
    sf::Vector2f anchorPoint;

    sf::Color flashColor, blendColor;
    sf::FloatRect texRect;
    float opacity;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
protected:
    sf::VertexArray vertices;
    virtual void setupVertices();

public:
    Sprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor);
    Sprite(std::shared_ptr<sf::Texture> tex);
    Sprite();
    
    virtual ~Sprite() {}

    sf::Vector2f getAnchorPoint() const { return anchorPoint; }
    void setAnchorPoint(sf::Vector2f ap) { anchorPoint = ap; }

    auto getTextureSize() const
    {
        return texture ? texture->getSize() : sf::Vector2u(0, 0);
    }
    
    virtual sf::FloatRect getBounds() const;

    auto getFlashColor() const { return flashColor; }
    void setFlashColor(sf::Color color) { flashColor = color; }

    auto getBlendColor() const { return blendColor; }
    void setBlendColor(sf::Color color) { blendColor = color; }

    auto getOpacity() const { return opacity; }
    void setOpacity(float op) { opacity = op; }

    auto getTexture() const { return texture; }
    virtual void setTexture(std::shared_ptr<sf::Texture> tex)
    { 
        texture = tex;
        texRect = sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(getTextureSize()));
        setupVertices();
    }
    
    auto getTextureRect() const { return texRect; }
    void setTextureRect(sf::FloatRect rect) { texRect = rect; setupVertices(); }
};
