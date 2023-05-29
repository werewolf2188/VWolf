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

#include "VWolf/Core/Debug/ShapeHelper.h"

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
        emptyMeshData = ShapeHelper::CreateEmpty();
    }

    Scene::Scene(Scene& scene) {
        this->name = scene.name;
        this->sceneBackGround = scene.sceneBackGround;
        this->m_registry = std::move(scene.m_registry);
        this->gameObjects = scene.gameObjects;
        for (auto gameObject: this->gameObjects) {
            gameObject->AttachToScene(this);
        }
        emptyMeshData = ShapeHelper::CreateEmpty();
    }

    Scene::Scene(Scene&& scene) {
        emptyMeshData = ShapeHelper::CreateEmpty();
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
        auto fun = [name](Ref<GameObject> object) {
            return object->GetName() == name;
        };
        auto res = std::find_if(gameObjects.begin(), gameObjects.end(), fun);
        if (res != gameObjects.end()) {
            std::stringstream st;
            st << "(" << gameObjects.size() << ")";
            name += st.str();
        }

        Ref<GameObject> gameObject = CreateRef<GameObject>( name, m_registry.create(), this );
        gameObject->AddComponent<TransformComponent>();
        gameObjects.push_back(gameObject);
        return gameObject;
    }

    void Scene::AddExistingGameObject(Ref<GameObject> gameObject) {
        gameObjects.push_back(gameObject);
    }

    void Scene::RemoveGameObject(std::string name) {
        auto fun = [name](Ref<GameObject> object) {
            return object->GetName() == name;
        };
        auto res = std::find_if(gameObjects.begin(), gameObjects.end(), fun);
        if (res != gameObjects.end()) {
            m_registry.destroy((*res)->GetHandle());
            gameObjects.erase(res);
        }
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


        auto transformComponents = m_registry.view<TransformComponent>();
        for (auto transformEntity : transformComponents)
        {
            // TODO: Should it be a renderer component? We just want to know if it has more than one component
            if (m_registry.try_get<ShapeRendererComponent>(transformEntity)) continue;
            auto transform = transformComponents
                .get<TransformComponent>(transformEntity);
            transform.Apply();
            Graphics::RenderMesh(emptyMeshData,
                                 transform.GetWorldMatrix(),
                                 *MaterialLibrary::Default());
        }
    }
}
