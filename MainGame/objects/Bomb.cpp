#include "Bomb.hpp"

#include "defaults.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "particles/ParticleBatch.hpp"

#include "objects/Player.hpp"

using namespace std::literals::chrono_literals;

constexpr auto DetonationTime = 48 * UpdateFrequency;
constexpr float SinePhase = 4;
constexpr float SinePower = 2;

Bomb::Bomb(GameScene& scene, cpVect pos, std::chrono::steady_clock::time_point initialTime)
    : GameObject(scene), position(pos), detonationTime(initialTime + DetonationTime),
    sprite(scene.getResourceManager().load<sf::Texture>("bomb.png"))
{
    
}

Bomb::~Bomb()
{
    auto player = gameScene.getObjectByName<Player>("player");
    if (player) player->numBombs++;
}

void Bomb::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    
    if (curTime > detonationTime)
    {
        detonate();
        remove();
    }
}

void Bomb::detonate()
{
    auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "bomb");
    batch->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(batch));
    
    gameScene.getGameSpace().pointQuery(position, 48, CP_SHAPE_FILTER_ALL,
    [=](cpShape* shape, cpVect point, cpFloat distance, cpVect gradient)
    {
        if (distance <= 48 && cpShapeGetCollisionType(shape) == Interactable)
            (*(GameObject::InteractionHandler*)cpShapeGetUserData(shape))(InteractionType, (void*)this);
    });
}

bool Bomb::notifyScreenTransition(cpVect displacement)
{
    position = position + displacement;
    return true;
}

void Bomb::render(Renderer& renderer)
{
    float factor = 1.0f - toSeconds<float>(detonationTime - curTime) / toSeconds<float>(DetonationTime);
    sprite.setOpacity(0.75f + 0.25f * cosf(2 * M_PI * SinePhase * powf(factor, SinePower)));
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 14);
    renderer.popTransform();
}
