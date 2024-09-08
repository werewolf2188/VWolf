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

#include "VWolf/Core/Physics/Physics.h"

#include "VWolf/Core/Time.h"

namespace VWolf {

    MeshData CreateSkyBox() {
        MeshData meshData;
        meshData.SetName("SkyBox");
        meshData.vertices.resize(8);
        
        /*
         
         float skyboxVertices[] =
         {
             //   Coordinates
             -1.0f, -1.0f,  1.0f,//        7--------6
              1.0f, -1.0f,  1.0f,//       /|       /|
              1.0f, -1.0f, -1.0f,//      4--------5 |
             -1.0f, -1.0f, -1.0f,//      | |      | |
             -1.0f,  1.0f,  1.0f,//      | 3------|-2
              1.0f,  1.0f,  1.0f,//      |/       |/
              1.0f,  1.0f, -1.0f,//      0--------1
             -1.0f,  1.0f, -1.0f
         };
         */

        meshData.vertices[0].position = VWolf::Vector3(-1.0f, -1.0f,  1.0f);
        meshData.vertices[1].position = VWolf::Vector3(1.0f, -1.0f,  1.0f);
        meshData.vertices[2].position = VWolf::Vector3(1.0f, -1.0f, -1.0f);
        meshData.vertices[3].position = VWolf::Vector3(-1.0f, -1.0f, -1.0f);
        meshData.vertices[4].position = VWolf::Vector3(-1.0f,  1.0f,  1.0f);
        meshData.vertices[5].position = VWolf::Vector3(1.0f,  1.0f,  1.0f);
        meshData.vertices[6].position = VWolf::Vector3(1.0f,  1.0f, -1.0f);
        meshData.vertices[7].position = VWolf::Vector3(-1.0f,  1.0f, -1.0f);

        unsigned int skyboxIndices[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3
        };
        meshData.indices.assign(&skyboxIndices[0], &skyboxIndices[36]);
        return meshData;
    }

    // ---------------- SCENE BACKGROUND ----------------
    SceneBackground::SceneBackground(): backgroundColor(Color(0.0f, 0.0f, 0.0f, 1.0f )), skybox(CreateSkyBox()) {}

    SceneBackground::SceneBackground(SceneBackground& scene): backgroundColor(scene.backgroundColor), skybox(CreateSkyBox()) {}
    

    SceneBackground::~SceneBackground() {}

    SceneBackground& SceneBackground::operator=(SceneBackground& t) {
        this->backgroundColor = t.backgroundColor;
        this->type = t.type;
        return *this;
    }

    SceneBackground& SceneBackground::operator=(SceneBackground&& t) {
        this->backgroundColor = t.backgroundColor;
        this->type = t.type;

        t.backgroundColor = Color();
        t.type = Type::Color;
        return *this;
    }

    // --------------- SCENE ----------------------------
    Scene::Scene(std::string name): name(name) {
        emptyMeshData = ShapeHelper::CreateEmpty();
        world = Physics::GetCommon().createPhysicsWorld();
        world->setIsDebugRenderingEnabled(true);
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
        world = Physics::GetCommon().createPhysicsWorld();
        world->setIsDebugRenderingEnabled(true);
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
        world = Physics::GetCommon().createPhysicsWorld();
        world->setIsDebugRenderingEnabled(true);
    }

    Scene::~Scene() {
        // TODO: Why is this failing?
//        if (world)
//            common.destroyPhysicsWorld(world);
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
        if (isPreviewing) {
            // Apply physics
            previewAccumulator += Time::GetDeltaTime();
            
//            while (previewAccumulator >= Physics::GetTimeStep()) {
                world->update(Physics::GetTimeStep());
//                previewAccumulator -= Physics::GetTimeStep();
//            }
//            float factor = previewAccumulator / Physics::GetTimeStep();
            testData.vertices.clear();
            testData.indices.clear();
            reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
            auto& triangles = debugRenderer.getTriangles();
            for (uint32_t index = 0; index < debugRenderer.getNbTriangles(); index++) {
                auto triangle = triangles[index];
                testData.vertices.push_back(Vertex(triangle.point1.x, triangle.point1.y, triangle.point1.z, 1, 1, 1, 1));
                testData.vertices.push_back(Vertex(triangle.point2.x, triangle.point2.y, triangle.point2.z, 1, 1, 1, 1));
                testData.vertices.push_back(Vertex(triangle.point3.x, triangle.point3.y, triangle.point3.z, 1, 1, 1, 1));

                testData.indices.push_back(index * 3);
                testData.indices.push_back((index * 3) + 1);
                testData.indices.push_back((index * 3) + 2);
            }
            auto meshColMeshFilterTrans = m_previewRegistry.view<MeshColliderComponent, MeshFilterComponent, TransformComponent>();
            
            for (auto meshColMeshFilterTransEnty : meshColMeshFilterTrans)
            {
                auto [meshCollider, meshFilter, transform] = meshColMeshFilterTrans
                    .get<MeshColliderComponent, MeshFilterComponent, TransformComponent>(meshColMeshFilterTransEnty);
                meshCollider.Update(transform);
            }

            auto sphereColMeshFilterTrans = m_previewRegistry.view<SphereColliderComponent, MeshFilterComponent, TransformComponent>();
            
            for (auto sphereColMeshFilterTransEnty : sphereColMeshFilterTrans)
            {
                auto [sphereCollider, meshFilter, transform] = sphereColMeshFilterTrans
                    .get<SphereColliderComponent, MeshFilterComponent, TransformComponent>(sphereColMeshFilterTransEnty);
                sphereCollider.Update(transform);
            }

            auto boxColMeshFilterTrans = m_previewRegistry.view<BoxColliderComponent, MeshFilterComponent, TransformComponent>();
            
            for (auto boxColMeshFilterTransEnty : boxColMeshFilterTrans)
            {
                auto [boxCollider, meshFilter, transform] = boxColMeshFilterTrans
                    .get<BoxColliderComponent, MeshFilterComponent, TransformComponent>(boxColMeshFilterTransEnty);
                boxCollider.Update(transform);
            }

            auto rigidBodyAndTransformComponents = m_previewRegistry.view<RigidBodyComponent, TransformComponent>();
            
            for (auto rigidBodyAndTransformEntity : rigidBodyAndTransformComponents)
            {
                auto [rigidBody, transform] = rigidBodyAndTransformComponents
                    .get<RigidBodyComponent, TransformComponent>(rigidBodyAndTransformEntity);
                rigidBody.Update(transform, 1);
            }

            auto audioListenerTrans = m_previewRegistry.view<AudioListenerComponent, TransformComponent>();

            if (audioListenerTrans.size_hint() > 0) {
                auto& listenerEntity = *audioListenerTrans.begin();
                auto [audioListener, listenerTransform]  = audioListenerTrans.get<AudioListenerComponent, TransformComponent>(listenerEntity);
                auto audioSourcesView = m_previewRegistry.view<AudioSourceComponent, TransformComponent>();
                for (auto audioSourceEntity: audioSourcesView) {
                    auto [audioSource, sourceTransform] = audioSourcesView.get<AudioSourceComponent, TransformComponent>(audioSourceEntity);
                    audioSource.Update(listenerTransform, sourceTransform);
                }
            }
        }
    }

    void Scene::StartingPreview() {
        isPreviewing = true;
        previewAccumulator = Time::GetDeltaTime();

        reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
        debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);

        for (auto gameObject: gameObjects) {
            Ref<GameObject> previewGameObject = CreateRef<GameObject>(gameObject->GetName(), m_previewRegistry.create(), this);
            previewGameObject->CopyComponents(gameObject.get());
            previewGameObjects.push_back(previewGameObject);
        }

        auto rigidBodyAndTransformComponents = m_previewRegistry.view<RigidBodyComponent, TransformComponent>();
        
        for (auto rigidBodyAndTransformEntity : rigidBodyAndTransformComponents)
        {
            auto [rigidBody, transform] = rigidBodyAndTransformComponents
                .get<RigidBodyComponent, TransformComponent>(rigidBodyAndTransformEntity);
            rigidBody.CreateRigidBody(world, transform);
        }

        auto meshColMeshFilterTrans = m_previewRegistry.view<MeshColliderComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto meshColMeshFilterTransEnty : meshColMeshFilterTrans)
        {
            auto [meshCollider, meshFilter, transform] = meshColMeshFilterTrans
                .get<MeshColliderComponent, MeshFilterComponent, TransformComponent>(meshColMeshFilterTransEnty);
            meshCollider.CreateMeshCollider(meshFilter.GetData(), transform);
        }

        auto sphereColMeshFilterTrans = m_previewRegistry.view<SphereColliderComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto sphereColMeshFilterTransEnty : sphereColMeshFilterTrans)
        {
            auto [sphereCollider, meshFilter, transform] = sphereColMeshFilterTrans
                .get<SphereColliderComponent, MeshFilterComponent, TransformComponent>(sphereColMeshFilterTransEnty);
            sphereCollider.CreateSphereCollider(meshFilter.GetData(), transform);
        }

        auto boxColMeshFilterTrans = m_previewRegistry.view<BoxColliderComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto boxColMeshFilterTransEnty : boxColMeshFilterTrans)
        {
            auto [boxCollider, meshFilter, transform] = boxColMeshFilterTrans
                .get<BoxColliderComponent, MeshFilterComponent, TransformComponent>(boxColMeshFilterTransEnty);
            boxCollider.CreateBoxCollider(meshFilter.GetData(), transform);
        }

        auto audioListenerTrans = m_previewRegistry.view<AudioListenerComponent, TransformComponent>();

        if (audioListenerTrans.size_hint() > 0) {
            auto& listenerEntity = *audioListenerTrans.begin();
            auto [audioListener, listenerTransform]  = audioListenerTrans.get<AudioListenerComponent, TransformComponent>(listenerEntity);
            auto audioSourcesView = m_previewRegistry.view<AudioSourceComponent, TransformComponent>();
            for (auto audioSourceEntity: audioSourcesView) {
                auto [audioSource, sourceTransform] = audioSourcesView.get<AudioSourceComponent, TransformComponent>(audioSourceEntity);
                audioSource.Prepare(listenerTransform, sourceTransform);
            }
        }
    }

    void Scene::StopingPreview() {
        isPreviewing = false;

        auto audioSourcesView = m_previewRegistry.view<AudioSourceComponent>();
        for (auto audioSourceEntity: audioSourcesView) {
            auto& audioSource = audioSourcesView.get<AudioSourceComponent>(audioSourceEntity);
            audioSource.End();
        }

        auto meshColMeshFilterTrans = m_previewRegistry.view<MeshColliderComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto meshColMeshFilterTransEnty : meshColMeshFilterTrans)
        {
            auto [meshCollider, meshFilter, transform] = meshColMeshFilterTrans
                .get<MeshColliderComponent, MeshFilterComponent, TransformComponent>(meshColMeshFilterTransEnty);
            meshCollider.Destroy();
        }

        auto boxColMeshFilterTrans = m_previewRegistry.view<BoxColliderComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto boxColMeshFilterTransEnty : boxColMeshFilterTrans)
        {
            auto [boxCollider, meshFilter, transform] = boxColMeshFilterTrans
                .get<BoxColliderComponent, MeshFilterComponent, TransformComponent>(boxColMeshFilterTransEnty);
            boxCollider.Destroy();
        }

        auto sphereColMeshFilterTrans = m_previewRegistry.view<SphereColliderComponent, MeshFilterComponent, TransformComponent>();

        for (auto sphereColMeshFilterTransEnty : sphereColMeshFilterTrans)
        {
            auto [sphereCollider, meshFilter, transform] = sphereColMeshFilterTrans
                .get<SphereColliderComponent, MeshFilterComponent, TransformComponent>(sphereColMeshFilterTransEnty);
            sphereCollider.Destroy();
        }

        auto rigidBodyAndTransformComponents = m_previewRegistry.view<RigidBodyComponent, TransformComponent>();
        
        for (auto rigidBodyAndTransformEntity : rigidBodyAndTransformComponents)
        {
            auto [rigidBody, transform] = rigidBodyAndTransformComponents
                .get<RigidBodyComponent, TransformComponent>(rigidBodyAndTransformEntity);
            rigidBody.DestroyRigidBody(world);
        }

        for (auto previewGameObject: previewGameObjects) {
            m_previewRegistry.destroy(previewGameObject->GetHandle());
        }
        previewGameObjects.clear();
    }

    void Scene::DrawEditor(Ref<Camera> editorCamera) {
        Graphics::ClearColor(sceneBackGround.GetBackgroundColor());
        Graphics::Clear();

        if (sceneBackGround.GetType() == SceneBackground::Type::Skybox) {
            // Immediate drawing so it does not belong to the queue
            Graphics::DrawMesh(sceneBackGround.GetSkyboxMeshData(),
                               VWolf::Vector4(),
                               VWolf::Vector4(),
                               sceneBackGround.GetSkyboxMaterial(),
                               sceneBackGround.GetCamera());
        }

        auto lightsAndTransformComponents = m_registry.view<LightComponent, TransformComponent>();
        
        for (auto lightAndTransformEntity : lightsAndTransformComponents)
        {
            auto [light, transform] = lightsAndTransformComponents
                .get<LightComponent, TransformComponent>(lightAndTransformEntity);
            light.GetLight().position = Vector4(transform.GetPosition().GetX(), transform.GetPosition().GetY(), transform.GetPosition().GetZ(), 1.0);
            light.GetLight().direction = Vector4(transform.GetEulerAngles().GetX(), transform.GetEulerAngles().GetY(), transform.GetEulerAngles().GetZ(), 0.0);
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

        auto meshFilterMeshRendererAndTransformComponents = m_registry
            .view<MeshRendererComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto meshFilterMeshRendererAndTransformEntity : meshFilterMeshRendererAndTransformComponents)
        {
            auto [meshRenderer, meshFilter, transform] = meshFilterMeshRendererAndTransformComponents
                .get<MeshRendererComponent, MeshFilterComponent, TransformComponent>(meshFilterMeshRendererAndTransformEntity);
            transform.Apply();
            Graphics::RenderMesh(meshFilter.GetData(),
                                 transform.GetWorldMatrix(),
                                 meshRenderer.GetMaterial());
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

    void Scene::DrawPreviewEditor() {
        Graphics::ClearColor(sceneBackGround.GetBackgroundColor());
        Graphics::Clear();

        auto cameraAndTransformComponents = m_previewRegistry.view<CameraComponent, TransformComponent>();
        if (cameraAndTransformComponents.begin() == cameraAndTransformComponents.end()) return; // There is no camera

        auto cameraAndTransformEntity = cameraAndTransformComponents.front();

        auto [cameraCom, cameraTransform] = cameraAndTransformComponents.get<CameraComponent, TransformComponent>(cameraAndTransformEntity);

        Ref<Camera> camera = cameraCom.GetCamera(cameraTransform);

        if (sceneBackGround.GetType() == SceneBackground::Type::Skybox) {
            // Immediate drawing so it does not belong to the queue
            sceneBackGround.GetCamera()->UpdateView(Vector3(), Quaternion::Euler(
                                                             radians(cameraTransform.GetEulerAngles().GetX()),
                                                             radians(cameraTransform.GetEulerAngles().GetY()),
                                                             radians(cameraTransform.GetEulerAngles().GetZ())
                                                         ));
            Graphics::DrawMesh(sceneBackGround.GetSkyboxMeshData(),
                               VWolf::Vector4(),
                               VWolf::Vector4(),
                               sceneBackGround.GetSkyboxMaterial(),
                               sceneBackGround.GetCamera());
        }

        auto lightsAndTransformComponents = m_previewRegistry.view<LightComponent, TransformComponent>();
        
        for (auto lightAndTransformEntity : lightsAndTransformComponents)
        {
            auto [light, transform] = lightsAndTransformComponents
                .get<LightComponent, TransformComponent>(lightAndTransformEntity);
            light.GetLight().position = Vector4(transform.GetPosition().GetX(), transform.GetPosition().GetY(), transform.GetPosition().GetZ(), 1.0);
            light.GetLight().direction = Vector4(transform.GetEulerAngles().GetX(), transform.GetEulerAngles().GetY(), transform.GetEulerAngles().GetZ(), 0.0);
            VWolf::Graphics::AddLight(light.GetLight());
        }

        // TODO: We should be looking for any renderer, not only shape renderer
        auto shapeRendererAndTransformComponents = m_previewRegistry.view<ShapeRendererComponent, TransformComponent>();
        
        for (auto shapeRendererAndTransformEntity : shapeRendererAndTransformComponents)
        {
            auto [shapeRenderer, transform] = shapeRendererAndTransformComponents
                .get<ShapeRendererComponent, TransformComponent>(shapeRendererAndTransformEntity);
            transform.Apply();
            Graphics::RenderMesh(shapeRenderer.GetData(),
                                 transform.GetWorldMatrix(),
                                 shapeRenderer.GetMaterial(),
                                 camera);
        }

        auto meshFilterMeshRendererAndTransformComponents = m_previewRegistry
            .view<MeshRendererComponent, MeshFilterComponent, TransformComponent>();
        
        for (auto meshFilterMeshRendererAndTransformEntity : meshFilterMeshRendererAndTransformComponents)
        {
            auto [meshRenderer, meshFilter, transform] = meshFilterMeshRendererAndTransformComponents
                .get<MeshRendererComponent, MeshFilterComponent, TransformComponent>(meshFilterMeshRendererAndTransformEntity);
            transform.Apply();
            Graphics::RenderMesh(meshFilter.GetData(),
                                 transform.GetWorldMatrix(),
                                 meshRenderer.GetMaterial(),
                                 camera);
        }

        auto transformComponents = m_previewRegistry.view<TransformComponent>();
        for (auto transformEntity : transformComponents)
        {
            // TODO: Should it be a renderer component? We just want to know if it has more than one component
            if (m_registry.try_get<ShapeRendererComponent>(transformEntity)) continue;
            auto transform = transformComponents
                .get<TransformComponent>(transformEntity);
            transform.Apply();
            Graphics::RenderMesh(emptyMeshData,
                                 transform.GetWorldMatrix(),
                                 *MaterialLibrary::Default(),
                                 camera);
        }

        // TODO: Debug renderer
        if (testData.vertices.size() == 0) return;
        Graphics::DrawMesh(testData,
                           VWolf::Vector4(),
                           VWolf::Vector4(),
                           *MaterialLibrary::GetMaterial("RainbowColor"),
                           camera);
        
//        VWOLF_CORE_INFO("Test");
    }
}
