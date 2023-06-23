//
//  SceneSerializer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/21/23.
//

#include "vwpch.h"
#include "VWolf/Core/Components/GameObject.h"
#include "VWolf/Core/Debug/ShapeHelper.h"
#include "SceneSerializer.h"

#include "yaml-cpp/yaml.h"

// GLOBAL VARIABLE OF THE CURRENT SCENE
VWolf::Scene * currentScene = nullptr;
//

#include "VWolf/Core/Math/Serialization/VMath.h"
#include "VWolf/Core/Components/Serialization/TransformComponent.h"
#include "VWolf/Core/Components/Serialization/LightComponent.h"
#include "VWolf/Core/Components/Serialization/ShapeRendererComponent.h"
#include "VWolf/Core/Components/Serialization/MeshFilterComponent.h"
#include "VWolf/Core/Components/Serialization/MeshRendererComponent.h"
#include "VWolf/Core/Components/Serialization/GameObject.h"
#include "Serialization/Scene.h"

namespace VWolf {
    void SceneSerializer::Serialize(Ref<Scene> scene, std::filesystem::path path) {
        YAML::Emitter out;
        out << *scene.get();
        std::ofstream fout(path.string());
        fout << out.c_str();
    }

    Ref<Scene> SceneSerializer::Deserialize(std::filesystem::path path) {
        constexpr const char * key = "Scene";
        constexpr const char * defaultName = "Untitled";
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CORE_ERROR("Failed to load .scene file '%s'\n     %s", path.string().c_str(), e.what());
            return CreateRef<Scene>(defaultName);
        }

        if (!data[key])
            return CreateRef<Scene>(defaultName);

        Scene scene = data[key].as<Scene>();
        Ref<Scene> sceneName = CreateFromRef<Scene>(scene);
        currentScene = nullptr;

        return sceneName;
    }
}
