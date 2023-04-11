//
//  RendererComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"
#include "VWolf/Core/Render/Material.h"

namespace VWolf {

    class RendererComponent: public Component {
    public:
        RendererComponent();
        RendererComponent(std::string name);
        ~RendererComponent();
    VWOLF_COMPONENT_INSPECTOR_DEFINE(RendererComponent);
    };

    // TODO: Test. Will remove later
    class ShapeRendererComponent: public RendererComponent {
    public:
        ShapeRendererComponent(MeshData data, Material& material);
        ~ShapeRendererComponent();
    public:
        virtual void OnInspector() override;
    public:
        MeshData& GetData() { return data; }
        Material& GetMaterial() { return material; }
    private:
        MeshData data;
        Material& material;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(ShapeRendererComponent);
    };
}
