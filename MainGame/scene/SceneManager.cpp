#include "SceneManager.hpp"
#include "Scene.hpp"
#include "rendering/Renderer.hpp"

SceneManager::SceneManager() : sceneStack(), deletionScheduled(false), postDeletionScene()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::pushScene(Scene *scene)
{
    scene->sceneManager = this;
    sceneStack.emplace(scene);
}

void SceneManager::popScene()
{
    deletionScheduled = true;
}

void SceneManager::replaceScene(Scene* scene)
{
    postDeletionScene.reset(scene);
    scene->sceneManager = this;
    deletionScheduled = true;
}

void SceneManager::handleScreenTransition()
{
    if (deletionScheduled)
    {
        sceneStack.top()->sceneManager = nullptr;
        if (postDeletionScene)
        {
            using std::swap;
            swap(postDeletionScene, sceneStack.top());
            postDeletionScene.reset();
        }
        else sceneStack.pop();

        deletionScheduled = false;
    }
}

void SceneManager::update(std::chrono::steady_clock::time_point curTime)
{
    handleScreenTransition();

    sceneStack.top()->update(curTime);
}

void SceneManager::render(Renderer& renderer)
{
    sceneStack.top()->render(renderer);
}
