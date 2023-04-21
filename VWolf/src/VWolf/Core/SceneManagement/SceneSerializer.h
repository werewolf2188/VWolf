//
//  SceneSerializer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/21/23.
//

#pragma once

#include "Scene.h"

namespace VWolf {

    class SceneSerializer {
    public:
        static void Serialize(Ref<Scene> scene, std::filesystem::path path);
        static Ref<Scene> Deserialize(std::filesystem::path path);
    };
}
