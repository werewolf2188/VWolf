//
//  MeshFilterComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/23/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"

#include "VWolf/Core/Utils/OBJLoader.h"

namespace VWolf {
    class MeshFilterComponent: public Component {
    public:
        MeshFilterComponent();
        MeshFilterComponent(std::filesystem::path data);
        MeshFilterComponent(MeshFilterComponent& component);
        MeshFilterComponent(MeshFilterComponent&& component);
        ~MeshFilterComponent();
    public:
        virtual void OnInspector() override;
    public:
        MeshData& GetData() { return data; }
        void SetPath(std::filesystem::path path) { this->path = path; Load(); }
        std::filesystem::path GetPath() { return this->path; }
    public:
        MeshFilterComponent& operator=(MeshFilterComponent t);
    private:
        void Load();
    private:
        MeshData data;
        std::filesystem::path path;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(MeshFilterComponent);
    };
}
