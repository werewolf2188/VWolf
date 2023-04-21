//
//  Scene.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/11/23.
//

#include "vwpch.h"

#include "VWolf/Core/Components/GameObject.h"

#include "Scene.h"

#include "VWolf/Core/Render/Graphics.h"

namespace VWolf {

    // ---------------- SCENE BACKGROUND ----------------
    SceneBackground::SceneBackground(): backgroundColor({ 0.0f, 0.0f, 0.0f, 1.0f }) {}

    SceneBackground::SceneBackground(SceneBackground& scene): backgroundColor(scene.backgroundColor) {}
    

    SceneBackground::~SceneBackground() {}

    SceneBackground& SceneBackground::operator=(SceneBackground& t) {
        this->backgroundColor = t.backgroundColor;
        return *this;
    }

    // --------------- SCENE ----------------------------
    Scene::Scene(std::string name): name(name) {
    }

    Scene::Scene(Scene& scene) {
        this->name = scene.name;
        this->sceneBackGround = scene.sceneBackGround;
        this->m_registry = std::move(scene.m_registry);
        this->gameObjects = scene.gameObjects;
        for (auto gameObject: this->gameObjects) {
            gameObject->AttachToScene(this);
        }
    }

    Scene::Scene(Scene&& scene) {
        this->name = scene.name;
        this->sceneBackGround = scene.sceneBackGround;
        this->m_registry = std::move(scene.m_registry);
        this->gameObjects = scene.gameObjects;
        for (auto gameObject: this->gameObjects) {
            gameObject->AttachToScene(this);
        }

        scene.name = std::string();
        scene.gameObjects.clear();
    }

    Scene::~Scene() {
        
    }

    Ref<GameObject> Scene::CreateGameObject(std::string name) {
        Ref<GameObject> gameObject = CreateRef<GameObject>( name, m_registry.create(), this );
        gameObject->AddComponent<TransformComponent>();
        gameObjects.push_back(gameObject);
        return gameObject;
    }

    void Scene::AddExistingGameObject(Ref<GameObject> gameObject) {
        gameObjects.push_back(gameObject);
    }
    
    void Scene::UpdateEditor() {
    }

    void Scene::DrawEditor(Ref<Camera> editorCamera) {
        Graphics::ClearColor(sceneBackGround.GetBackgroundColor());
        Graphics::Clear();
        auto lightsAndTransformComponents = m_registry.view<LightComponent, TransformComponent>();
        
        for (auto lightAndTransformEntity : lightsAndTransformComponents)
        {
            auto [light, transform] = lightsAndTransformComponents
                .get<LightComponent, TransformComponent>(lightAndTransformEntity);
            light.GetLight().position = Vector4Float(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z, 1.0);
            light.GetLight().direction = Vector4Float(transform.GetEulerAngles().x, transform.GetEulerAngles().y, transform.GetEulerAngles().z, 0.0);
            VWolf::Graphics::AddLight(light.GetLight());
        }

        // TODO: We should be looking for any renderer, not only shape renderer
        auto shapeRendererAndTransformComponents = m_registry.view<ShapeRendererComponent, TransformComponent>();
        
        for (auto shapeRendererAndTransformEntity : shapeRendererAndTransformComponents)
        {
            auto [shapeRenderer, transform] = shapeRendererAndTransformComponents
                .get<ShapeRendererComponent, TransformComponent>(shapeRendererAndTransformEntity);
            transform.Apply();
            Graphics::RenderMesh(shapeRenderer.GetData(),
                                 transform.GetWorldMatrix(),
                                 shapeRenderer.GetMaterial());
        }
        VWolf::Graphics::DrawGrid();
    }
}
