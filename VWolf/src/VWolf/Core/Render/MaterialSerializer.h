//
//  MaterialSerializer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/2/24.
//

#pragma once

#include "Material.h"

namespace VWolf {

    class MaterialSerializer {
    public:
        static void Serialize(Material& material, std::filesystem::path path);
        static Ref<Material> Deserialize(std::filesystem::path path);
    };
}
