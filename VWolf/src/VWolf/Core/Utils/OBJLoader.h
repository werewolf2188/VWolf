//
//  OBJLoader.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/13/23.
//

#pragma once

#include "VWolf/Core/Render/RenderStructs.h"

namespace VWolf {
    class OBJLoader {
    public:
        static MeshData Load(std::string path, std::string name);
    };
}
