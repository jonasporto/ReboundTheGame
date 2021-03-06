#pragma once

#include <non_copyable_movable.hpp>

#include <stack>
#include <memory>
#include <chrono>

class Scene;
class Renderer;

class SceneManager final : util::non_copyable_movable
{
    std::stack<std::unique_ptr<Scene>> sceneStack;
    bool deletionScheduled;
    std::unique_ptr<Scene> postDeletionScene;

public:
    SceneManager();
    ~SceneManager();

    void pushScene(Scene* scene);
    void popScene();
    void replaceScene(Scene* scene);

    void handleScreenTransition();

    inline bool hasScenes() const { return !sceneStack.empty(); }

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
};
