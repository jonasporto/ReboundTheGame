#include "GUIMap.hpp"

#include <SFML/OpenGL.hpp>
#include <assert.hpp>
#include <chronoUtils.hpp>
#include <rectUtils.hpp>
#include "defaults.hpp"
#include "gameplay/MapGenerator.hpp"
#include "data/LevelData.hpp"

const sf::FloatRect MapViewport(-38, -38, 76, 76);
const sf::Color BlinkColor = sf::Color(255, 128, 128, 255);
constexpr float BlinkPeriod = 2;

constexpr uint8_t PresentSpeed = 6;

void setColorOnly(sf::Color& color, sf::Color dest)
{
    color.r = dest.r;
    color.g = dest.g;
    color.b = dest.b;
}

void GUIMap::update(std::chrono::steady_clock::time_point curTime)
{
    if (initTime == decltype(initTime)()) initTime = curTime;
    
    float t = toSeconds<float>(curTime - initTime);
    float weight = 0.5 - 0.5 * cosf(2 * M_PI * t / BlinkPeriod);
    sf::Color finalColor(255 + weight * ((int)BlinkColor.r - 255),
                         255 + weight * ((int)BlinkColor.g - 255),
                         255 + weight * ((int)BlinkColor.b - 255),
                         255 + weight * ((int)BlinkColor.a - 255));
                        
    if (curLevel)
        for (size_t i = 0; i < curLevel->roomMaps.size(); i++)
            for (size_t k = 0; k < 6; k++)
            {
                setColorOnly(vertArray[6*i+k].color, curRoom == i ? finalColor : sf::Color::White);
                if (vertArray[6*i+k].color.a != 0 && vertArray[6*i+k].color.a != 255)
                    vertArray[6*i+k].color.a = std::min((size_t)vertArray[6*i+k].color.a + PresentSpeed, (size_t)255);
            }
}

void GUIMap::setCurLevel(std::shared_ptr<LevelData> level)
{
    if (curLevel != level)
    {
        curLevel = level;
        buildLevelTexture();
    }
}

void GUIMap::buildLevelTexture()
{
    if (!curLevel)
    {
        vertArray.clear();
        return;
    }
    
    sf::IntRect bounds;
    vertArray.resize(6 * curLevel->roomMaps.size());
    
    for (const auto& mapData : curLevel->roomMaps)
        bounds = rectUnionWithRect(bounds,
            sf::IntRect(mapData.x, mapData.y, mapData.map.width(), mapData.map.height()));
            
    ASSERT(curTexture.create(bounds.width, bounds.height));
    
    auto buildVertex = [=](float x, float y)
    {
        return sf::Vertex(sf::Vector2f(x, y), sf::Color(255, 255, 255, 0), sf::Vector2f(x - bounds.left, y - bounds.top));
    };
    
    size_t i = 0;
    for (const auto& mapData : curLevel->roomMaps)
    {
        if (mapData.map.empty()) continue;
        curTexture.update(getTextureData(mapData.map).get(), mapData.map.width(), mapData.map.height(),
            mapData.x - bounds.left, mapData.y - bounds.top);
        
        vertArray[i++] = buildVertex(mapData.x, mapData.y);
        vertArray[i++] = buildVertex(mapData.x + (int16_t)mapData.map.width(), mapData.y);
        vertArray[i++] = buildVertex(mapData.x + (int16_t)mapData.map.width(), mapData.y + (int16_t)mapData.map.height());
        vertArray[i++] = buildVertex(mapData.x, mapData.y);
        vertArray[i++] = buildVertex(mapData.x + (int16_t)mapData.map.width(), mapData.y + (int16_t)mapData.map.height());
        vertArray[i++] = buildVertex(mapData.x, mapData.y + (int16_t)mapData.map.height());
    }
}

void GUIMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    auto scissorRect = states.transform.transformRect(MapViewport);
    
    states.transform.translate(-curLevel->roomMaps[curRoom].x, -curLevel->roomMaps[curRoom].y);
    states.transform.translate(-displayPosition);
    states.texture = &curTexture;
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(scissorRect.left, ScreenHeight - scissorRect.top - scissorRect.height, scissorRect.width, scissorRect.height);
    target.draw(vertArray, states);
    glDisable(GL_SCISSOR_TEST);
}

void GUIMap::presentRoom(size_t room)
{
    for (size_t k = 0; k < 6; k++)
        if (vertArray[6*room+k].color.a == 0)
            vertArray[6*room+k].color.a = PresentSpeed;
}
    
void GUIMap::presentRoomFull(size_t room)
{
    for (size_t k = 0; k < 6; k++)
        vertArray[6*room+k].color.a = 255;
}
