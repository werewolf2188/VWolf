//
//  SceneObjects.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/23/23.
//

#pragma once

enum class SceneObjectsConstantKeys {
    Scene, SceneName, SceneBackground, BackgroundColor, BackgroundType, GameObjects
};

static std::map<SceneObjectsConstantKeys, const char*> sceneKeys = {
    { SceneObjectsConstantKeys::Scene, "Scene" },
    { SceneObjectsConstantKeys::SceneName, "Name" },
    { SceneObjectsConstantKeys::SceneBackground, "Scene_Background" },
    { SceneObjectsConstantKeys::BackgroundColor, "Background_color" },
    { SceneObjectsConstantKeys::BackgroundType, "Background_type" },
    { SceneObjectsConstantKeys::GameObjects, "Game_Objects" },
};

namespace YAML {
    template<>
    struct convert<VWolf::Scene>
    {
        static bool decode(const Node& node, VWolf::Scene& rhs)
        {
            if (!node.IsMap() ||
                !node[sceneKeys[SceneObjectsConstantKeys::SceneName]])
                return false;

            currentScene = &rhs;

            rhs.SetName(node[sceneKeys[SceneObjectsConstantKeys::SceneName]].as<std::string>());
    
            if (node[sceneKeys[SceneObjectsConstantKeys::SceneBackground]]) {
                auto background = node[sceneKeys[SceneObjectsConstantKeys::SceneBackground]]
                    .as<VWolf::SceneBackground>();
                rhs.SetSceneBackground(background);
            }

            if (node[sceneKeys[SceneObjectsConstantKeys::GameObjects]]) {
                for (auto gameObjectKeyValue: node[sceneKeys[SceneObjectsConstantKeys::GameObjects]]) {
                    VWolf::GameObject object = gameObjectKeyValue.as<VWolf::GameObject>();
                    VWolf::Ref<VWolf::GameObject> gameObject = VWolf::CreateFromRef<VWolf::GameObject>(object);
                    rhs.AddExistingGameObject(gameObject);
                }
            }
            
            return true;
        }
    };

    template<>
    struct convert<VWolf::SceneBackground>
    {
        static bool decode(const Node& node, VWolf::SceneBackground& rhs)
        {
            if (!node.IsMap() ||
                !node[sceneKeys[SceneObjectsConstantKeys::BackgroundColor]])
                return false;

            rhs.SetBackgroundColor(node[sceneKeys[SceneObjectsConstantKeys::BackgroundColor]].as<VWolf::Color>());
            if (node[sceneKeys[SceneObjectsConstantKeys::BackgroundType]]) {
                rhs.SetType((VWolf::SceneBackground::Type)node[sceneKeys[SceneObjectsConstantKeys::BackgroundType]]
                                       .as<int>());
            } else rhs.SetType(VWolf::SceneBackground::Type::Color);
            
            return true;
        }
    };
}

namespace VWolf {

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::SceneBackground& v)
    {
        out << YAML::Key << sceneKeys[SceneObjectsConstantKeys::SceneBackground];
        out << YAML::BeginMap;
        out << YAML::Key << sceneKeys[SceneObjectsConstantKeys::BackgroundColor] << YAML::Value << v.GetBackgroundColor();
        out << YAML::Key << sceneKeys[SceneObjectsConstantKeys::BackgroundType] << YAML::Value << (int)v.GetType();
        out << YAML::EndMap;
        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Scene& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << sceneKeys[SceneObjectsConstantKeys::Scene];
        out << YAML::BeginMap;
        out << YAML::Key << sceneKeys[SceneObjectsConstantKeys::SceneName] << YAML::Value << v.GetName();
        out << v.GetSceneBackground();
        out << YAML::Key << sceneKeys[SceneObjectsConstantKeys::GameObjects];
        out << YAML::BeginSeq;
        for (auto gameObject: v.GetGameObjects()) {
            out << *gameObject.get();
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
}
