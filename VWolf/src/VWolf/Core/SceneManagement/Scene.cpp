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
#include "VWolf/Core/Render/GraphicsContext.h"
#include "VWolf/Core/Render/InternalGraphics.h"
#include "VWolf/Core/Debug/ShapeHelper.h"
#include "VWolf/Core/Physics/Physics.h"
#include "VWolf/Core/Time.h"

const std::string sceneBackgroundKey = "sceneBackGround";
const std::string gameObjectsKey = "gameObjects";
const std::string sceneKey = "Scene";

namespace YAML {
    bool DeserializeGameObjects(const Node& node, VWolf::Scene& rhs) {
        for (auto gameObjectKeyValue: node[gameObjectsKey]) {
            VWolf::GameObject object = gameObjectKeyValue.as<VWolf::GameObject>();
            VWolf::Ref<VWolf::GameObject> gameObject = VWolf::CreateFromRef<VWolf::GameObject>(object);
            gameObject->AttachToScene(rhs.weak_from_this());
            convert<VWolf::GameObject>::decodeComponents(gameObjectKeyValue, *gameObject.get());
            rhs.AddExistingGameObject(gameObject);
        }
        return true;
    }
}

namespace VWolf {

    // ---------------- SCENE BACKGROUND ----------------
    SceneBackground::SceneBackground(): backgroundColor(Color(0.0f, 0.0f, 0.0f, 1.0f )), skyboxEx(CreateRef<Mesh>(ShapeHelper::CreateSkybox())) {}

    SceneBackground::SceneBackground(const SceneBackground& scene): backgroundColor(scene.backgroundColor), skyboxEx(CreateRef<Mesh>(ShapeHelper::CreateSkybox())) {}
    
    SceneBackground::~SceneBackground() {}

    SceneBackground& SceneBackground::operator=(const SceneBackground& t) {
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

    void SceneBackground::Draw(Ref<CameraComponent> camera) {
        Graphics::DrawMesh(GetSkyboxMesh(),
                           Vector3::Zero,
                           Quaternion::Identity,
                           GetSkyboxMaterialEx(),
                           0,
                           0,
                           camera,
                           false,
                           false);
    }

    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(VWolf::SceneBackground::Type, Color, Skybox)

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::SceneBackground& v)
    {
        SerializeFromBoostDescribeNoName(out, v);
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const VWolf::SceneBackground& v)
    {
        SerializeFromBoostDescribeNoName(out, v);
        return out;
    }

    // --------------- SCENE ----------------------------

    Scene::Scene(std::string name): Object(UUID::NewUUID()){
        this->name = name;
        emptyMesh = CreateRef<Mesh>(ShapeHelper::CreateEmpty());
        world = Physics::GetCommon().createPhysicsWorld();
        world->setIsDebugRenderingEnabled(true);
    }

    Scene::Scene(const Scene& scene): Object(scene.id) {
        this->name = scene.name;
        this->sceneBackGround = scene.sceneBackGround;
        this->m_registry.swap(const_cast<Scene&>(scene).m_registry);
        this->gameObjects = scene.gameObjects;
        emptyMesh = CreateRef<Mesh>(ShapeHelper::CreateEmpty());
        world = Physics::GetCommon().createPhysicsWorld();
        world->setIsDebugRenderingEnabled(true);
    }

    Scene::~Scene() {
        // TODO: Why is this failing?
//        if (world)
//            common.destroyPhysicsWorld(world);
    }

    Scene& Scene::operator=(const Scene& scene) {
        this->name = scene.name;
        this->sceneBackGround = scene.sceneBackGround;
        this->m_registry.swap(const_cast<Scene&>(scene).m_registry);
        this->gameObjects = scene.gameObjects;
        for (auto gameObject: this->gameObjects) {
            gameObject->AttachToScene(this->weak_from_this());
        }
        emptyMesh = CreateRef<Mesh>(ShapeHelper::CreateEmpty());
        world = Physics::GetCommon().createPhysicsWorld();
        world->setIsDebugRenderingEnabled(true);
        
        return *this;
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

        Ref<GameObject> gameObject = CreateRef<GameObject>( name, m_registry.create(), weak_from_this());
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
            (*res)->ClearCurrentComponents();
            m_registry.destroy((*res)->GetHandle());
            gameObjects.erase(res);
        }
    }

    template<typename Type, typename... Other, typename Func, typename... Exclude>
    void CreateViewAndProcessComponents(entt::registry& registry, Func&& execute, entt::exclude_t<Exclude...> ex = entt::exclude_t{}) {
        auto view = registry.view<Type, Other...>(ex);
        for (auto entity : view)
        {
            auto tuple = view.template get<Type, Other...>(entity);
            std::forward<Func>(execute)(tuple);
        }
    }

    template<typename Type, typename... Other, typename Func, typename... Exclude>
    void CreateViewWithViewAndProcessComponents(entt::registry& registry, Func&& execute, entt::exclude_t<Exclude...> ex = entt::exclude_t{}) {
        auto view = registry.view<Type, Other...>(ex);
        for (auto entity : view)
        {
            auto tuple = view.template get<Type, Other...>(entity);
            std::forward<Func>(execute)(entity, tuple);
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
            
            CreateViewWithViewAndProcessComponents<MeshFilterComponent, TransformComponent>(m_previewRegistry, [this](auto& entity, auto& tuple){
                TransformComponent& transform = std::get<1>(tuple);
                
                if (auto* meshCollider = m_previewRegistry.try_get<MeshColliderComponent>(entity)) {
                    meshCollider->Update(transform);
                }
                if (auto* sphereCollider = m_previewRegistry.try_get<SphereColliderComponent>(entity)) {
                    sphereCollider->Update(transform);
                }
                if (auto* boxCollider = m_previewRegistry.try_get<BoxColliderComponent>(entity)) {
                    boxCollider->Update(transform);
                }
            });
            
            CreateViewAndProcessComponents<RigidBodyComponent, TransformComponent>(m_previewRegistry, [this](auto& tuple){
                std::get<0>(tuple).Update(std::get<1>(tuple), 1);
            });

            CreateViewAndProcessComponents<AudioListenerComponent, TransformComponent>(m_previewRegistry, [this](auto& tupleListener) {
                CreateViewAndProcessComponents<AudioSourceComponent, TransformComponent>(m_previewRegistry, [this, tupleListener](auto& tuple) {
                    TransformComponent& listenerTransform = std::get<1>(tupleListener);
                    AudioSourceComponent& source = std::get<0>(tuple);
                    TransformComponent& sourceTransform = std::get<1>(tuple);
                    source.Update(listenerTransform, sourceTransform);
                });
            });
        }
    }

    void Scene::StartingPreview() {
        isPreviewing = true;
        previewAccumulator = Time::GetDeltaTime();

        for (auto gameObject: gameObjects) {
            if ((gameObject->GetFlags() & HideFlags::Editor) == HideFlags::Editor) continue;
            
            Ref<GameObject> previewGameObject = CreateRef<GameObject>(gameObject->GetName(), m_previewRegistry.create(), weak_from_this());
            previewGameObject->CopyComponents(gameObject);
            previewGameObjects.push_back(previewGameObject);
        }

        CreateViewAndProcessComponents<RigidBodyComponent, TransformComponent>(m_previewRegistry, [this](auto& tuple){
            std::get<0>(tuple).CreateRigidBody(world, std::get<1>(tuple));
        });
        
        CreateViewWithViewAndProcessComponents<MeshFilterComponent, TransformComponent>(m_previewRegistry, [this](auto& entity, auto& tuple){
            Ref<Mesh> mesh = std::get<0>(tuple).GetMesh();
            TransformComponent& transform = std::get<1>(tuple);
            
            if (auto* meshCollider = m_previewRegistry.try_get<MeshColliderComponent>(entity)) {
                meshCollider->CreateMeshCollider(mesh, transform);
            }
            if (auto* sphereCollider = m_previewRegistry.try_get<SphereColliderComponent>(entity)) {
                sphereCollider->CreateSphereCollider(mesh, transform);
            }
            if (auto* boxCollider = m_previewRegistry.try_get<BoxColliderComponent>(entity)) {
                boxCollider->CreateBoxCollider(mesh, transform);
            }
        });
        
        CreateViewAndProcessComponents<AudioListenerComponent, TransformComponent>(m_previewRegistry, [this](auto& tupleListener) {
            CreateViewAndProcessComponents<AudioSourceComponent, TransformComponent>(m_previewRegistry, [this, tupleListener](auto& tuple) {
                TransformComponent& listenerTransform = std::get<1>(tupleListener);
                AudioSourceComponent& source = std::get<0>(tuple);
                TransformComponent& sourceTransform = std::get<1>(tuple);
                source.Prepare(listenerTransform, sourceTransform);
            });
        });
    }

    void Scene::StopingPreview() {
        isPreviewing = false;

        CreateViewAndProcessComponents<AudioSourceComponent>(m_previewRegistry, [this](auto& tuple) {
            AudioSourceComponent& source = tuple;
            source.End();
        });
        
        CreateViewWithViewAndProcessComponents<MeshFilterComponent, TransformComponent>(m_previewRegistry, [this](auto& entity, auto& tuple){
            if (auto* meshCollider = m_previewRegistry.try_get<MeshColliderComponent>(entity)) {
                meshCollider->Destroy();
            }
            if (auto* sphereCollider = m_previewRegistry.try_get<SphereColliderComponent>(entity)) {
                sphereCollider->Destroy();
            }
            if (auto* boxCollider = m_previewRegistry.try_get<BoxColliderComponent>(entity)) {
                boxCollider->Destroy();
            }
        });
        
        CreateViewAndProcessComponents<RigidBodyComponent, TransformComponent>(m_previewRegistry, [this](auto& tuple){
            std::get<0>(tuple).DestroyRigidBody(world);
        });

        for (auto previewGameObject: previewGameObjects) {
            // Drop observers before registry destroys component storage.
            previewGameObject->ClearCurrentComponents();
            m_previewRegistry.destroy(previewGameObject->GetHandle());
        }
        previewGameObjects.clear();
        FindNextCamera();
    }

    void Scene::DrawEditor() {
        auto cameraAndTransformComponents = m_registry.view<CameraComponent, TransformComponent>();
        if (cameraAndTransformComponents.begin() == cameraAndTransformComponents.end()) return; // There is no camera

        entt::entity cameraAndTransformEntity;
        for(auto entity : cameraAndTransformComponents) {
            Ref<GameObject> gameObject = cameraAndTransformComponents.get<TransformComponent>(entity).GetGameObject();
            if ((gameObject->GetFlags() & HideFlags::Editor) != HideFlags::Editor) continue;
            cameraAndTransformEntity = entity;
        }

        auto [cameraCom, cameraTransform] = cameraAndTransformComponents.get<CameraComponent, TransformComponent>(cameraAndTransformEntity);

        Draw(UnownedRef<CameraComponent>(&cameraCom));
    }

    void Scene::Draw(Ref<CameraComponent> camera) {
        entt::registry& currentRegistry = isPreviewing ? m_previewRegistry : m_registry;
        
        GraphicsContext::SetClearColor(sceneBackGround.GetBackgroundColor());

        if (sceneBackGround.GetType() == SceneBackground::Type::Skybox) {
            sceneBackGround.Draw(camera);
        }
        
        CreateViewWithViewAndProcessComponents<TransformComponent>(currentRegistry, [this, camera, &currentRegistry](auto& entity, auto& tuple){
            TransformComponent& transform = tuple;
            transform.Apply();
            if (auto* light = currentRegistry.try_get<LightComponent>(entity)) {
                GraphicsContext::AddLight(*light);
            }
            bool hasRenderer = false;
            if (auto* shapeRenderer = currentRegistry.try_get<ShapeRendererComponent>(entity)) {
                Graphics::DrawMesh(shapeRenderer->GetMesh(),
                                   transform.GetWorldMatrix(),
                                   shapeRenderer->GetMaterialEx(),
                                   0,
                                   0,
                                   camera);
                hasRenderer = true;
            }
            if (auto* meshRenderer = currentRegistry.try_get<MeshRendererComponent>(entity)) {
                if (auto* meshFilter = currentRegistry.try_get<MeshFilterComponent>(entity)) {
                    Graphics::DrawMesh(meshFilter->GetMesh(),
                                       transform.GetWorldMatrix(),
                                       meshRenderer->GetMaterialEx(),
                                       0,
                                       0,
                                       camera);
                    hasRenderer = true;
                }
            }
            
            if (!hasRenderer)
                Graphics::DrawMesh(emptyMesh,
                                   transform.GetWorldMatrix(),
                                   MaterialLibrary::Default(),
                                   0,
                                   0,
                                   camera);
        });
    }

    void Scene::FindNextCamera() {
        // TODO: Main camera for preview mode should be the copy of the main camera in editing mode
        for(auto gameObject: (isPreviewing ? previewGameObjects : gameObjects)) {
            if (gameObject->HasComponent<CameraComponent>() && (gameObject->GetFlags() & HideFlags::Editor) == HideFlags::None) {
                CameraComponent::SetMainCamera(UnownedRef<CameraComponent>(&gameObject->GetComponent<CameraComponent>()));
                break;
            }
        }
    }

    void Scene::DrawPreviewEditor() {
        auto cameraAndTransformComponents = m_previewRegistry.view<CameraComponent, TransformComponent>();
        if (cameraAndTransformComponents.begin() == cameraAndTransformComponents.end()) return; // There is no camera

        entt::entity cameraAndTransformEntity;
        for(auto entity : cameraAndTransformComponents) {
            Ref<GameObject> gameObject = cameraAndTransformComponents.get<TransformComponent>(entity).GetGameObject();
            if ((gameObject->GetFlags() & HideFlags::Editor) == HideFlags::Editor) continue;
            cameraAndTransformEntity = entity;
        }

        auto [cameraCom, cameraTransform] = cameraAndTransformComponents.get<CameraComponent, TransformComponent>(cameraAndTransformEntity);
        
        Draw(UnownedRef<CameraComponent>(&cameraCom));
    }

    void Scene::Save(std::filesystem::path path) {
        YAML::Emitter out;
        out << *this;
        std::ofstream fout(path.string());
        fout << out.c_str();
    }

    Ref<Scene> Scene::Load(std::filesystem::path path, UUID _id) {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CORE_ERROR("Failed to load .scene file '%s'\n     %s", path.string().c_str(), e.what());
            throw;
        }

        if (!data[sceneKey])
            throw;
        
        const YAML::Node sceneNode = data[sceneKey];
        Scene scene = sceneNode.as<Scene>();
        scene.id = _id;
        Ref<Scene> sceneRef = CreateRef<Scene>(scene);
        YAML::DeserializeGameObjects(sceneNode, *sceneRef.get());
        ObjectResourceManager::AddObject(_id, sceneRef);
        return sceneRef;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Scene& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << sceneKey;
        out << YAML::BeginMap;
        SerializeFromBoostOnlyMembers(out, v);
        out << YAML::Key << gameObjectsKey;
        out << YAML::BeginSeq;
        for (auto gameObject: v.GetGameObjects()) {
            if ((gameObject->GetFlags() & HideFlags::DontSave) == HideFlags::None)
                out << *gameObject.get();
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
        out << YAML::EndMap;
        
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const VWolf::Scene& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << sceneKey;
        out << YAML::BeginMap;
        SerializeFromBoostOnlyMembers(out, v);
        out << YAML::Key << gameObjectsKey;
        out << YAML::BeginSeq;
        for (auto gameObject: v.GetGameObjects()) {
            if ((gameObject->GetFlags() & HideFlags::DontSave) == HideFlags::None)
                out << *gameObject.get();
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
        out << YAML::EndMap;
        
        return out;
    }
}
